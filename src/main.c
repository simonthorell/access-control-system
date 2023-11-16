#include <stdio.h>           // printf
#include <stdbool.h>         // bool
#include <string.h>          // strcmp (check pw)
#include <stdlib.h>          // free (memory deallocation)
#include <pthread.h>         // Threads
#include <fcntl.h>           // open (serial port)
#include <unistd.h>          // close (serial port)

#include "admin_menu.h"      // Admin console UI menu
#include "card_reader.h"     // RFID card reader functions (RFID MCU connected via serial port)
#include "card_management.h" // Struct for access cards
#include "data_storage.h"    // Retrieve access cards from file & save to file (.csv)
#include "connect_tcp_ip.h"  // Connect to door controller MCU wireless via TCP/IP
#include "connect_serial.h"  // Connect to RFID reader MCU via serial port

#include "util_sleep.h"      // portableSleep - TEMPORARY FIX TO RESET CARD READER

typedef struct {
    size_t *pCardsMallocated;
    size_t *pCardCount;
    accessCard *pAccessCards;
    Configuration *pConfig;             // Pointer to Configuration struct
    unsigned long int *pCardRead;            // Last read card number from MCU RFID card reader
    volatile bool runCardReaderThread;  // TODO: Change to atomic bool
} ThreadArgs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool isCardReaderReady = false; // This boolean represents the condition

void startThreads(ThreadArgs *args); // Start multithreading with 2 threads.
void *runCardReader(void *args);     // Thread 1: MCU Card reader (Arduino/ESP32)
void *runAdminConsol(void *args);    // Thread 2: Admin console UI (Linux/Windows/Mac)

int main(void) {
    printf("\n\033[1m*** DOOR ACCESS CONTROL SYSTEM ***\033[0m\n");

    // Load access cards from file into memory (heap)
    size_t cardsMallocated = 10; // Initial value to alloc for amount of cards
    size_t *pCardsMallocated = &cardsMallocated;
    size_t cardCount = 0; // retrieveAccessCards() will count lines & update cardCount
    size_t *pCardCount = &cardCount; 
    accessCard *pAccessCards = retrieveAccessCards(&cardsMallocated, &cardCount); // Do not forget to free memory

    unsigned long int cardRead = 0;           // Card number read by MCU RFID card reader
    unsigned long int *pCardRead = &cardRead; // Pointer to last card read by MCU RFID card reader.

    // Check if access cards were loaded successfully from file to heap.
    if (pAccessCards == NULL) {
        fprintf(stderr, "\033[31m* Failed to load access cards. Exiting program...\033[0m\n");
        return 1;
    } else {
        printf("\033[32m* Loaded %zu access cards from file 'access_cards.csv' into memory location: %p\033[0m\n", cardCount, (void *)pAccessCards);
    }

    // Load hardware specifications from 'config.ini' file into memory (s)
    Configuration *pConfig = readConfig("config.ini"); // data_storage.c
     if (pConfig != NULL) {
        ThreadArgs args;
        memset(&args, 0, sizeof(args)); // Initialize args to zero
        args.pConfig = pConfig;         // Set the config pointer in ThreadArgs
    }

    // Connect to the ESP8266 MCU on the door controller via TCP/IP
    tcpConnect(pConfig->door_ip_address); 

    // Start multithreading - 1. MCU Card reader, 2. Admin console UI
    ThreadArgs args = {pCardsMallocated, pCardCount, pAccessCards, pConfig, pCardRead, true};
    startThreads(&args);

    // close TCP/IP connection
    closeConnection();

    // save access cards to file
    saveAccessCards(pAccessCards, *pCardCount); // data_storage.c
    printf("\033[32m* Saved %zu access cards to file 'access_cards.csv'\033[0m\n", *pCardCount);

    // Clean up memory
    free(pAccessCards);  // Free memory allocated by retrieveAccessCards() in data_storage.c
    free(pConfig);       // Free memory allocated by readConfig() in data_storage.c
    printf("\033[32m* Memory deallocated successfully!\033[0m\n");
    printf("\033[1;32m*** PROGRAM EXITED SUCCESSFULLY ***\033[0m\n");

    return EXIT_SUCCESS;
}

void startThreads(ThreadArgs *args) {
    pthread_t thread1, thread2;

    // Create the card reader thread
    if (pthread_create(&thread1, NULL, runCardReader, (void *)args)) {
        perror("Error creating card reader thread");
        exit(EXIT_FAILURE);
    }

    // Ensure the card reader thread has initialzed serial port before starting admin console thread.
    pthread_mutex_lock(&mutex);
    while (!isCardReaderReady) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    // Create the admin console thread
    if (pthread_create(&thread2, NULL, runAdminConsol, (void *)args)) {
        perror("Error creating admin console thread");
        exit(EXIT_FAILURE);
    }

    // Wait for the threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}

void *runCardReader(void *args) {
    ThreadArgs *actualArgs = (ThreadArgs *)args;
    pthread_mutex_lock(&mutex);

    // Connect to the RFID reader on the serial port
    int serial_port = serialConnect(actualArgs->pConfig->rfid_serial_port); // card_reader.c

    // If the serial port could not be opened, exit the thread
    if (serial_port == -1) {
        actualArgs->runCardReaderThread = false;
        isCardReaderReady = true;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        printf("\033[36m* Running RFID reader simulation mode...\033[0m\n");
        return NULL;
    } else {
        isCardReaderReady = true;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    
    // Run the MCU card reader until the admin console request to shut down the system.
    while (actualArgs->runCardReaderThread) {
        // This will re-loop everytime a card has been read by the MCU RFID card reader.
        *actualArgs->pCardRead = rfidReading(actualArgs->pAccessCards, actualArgs->pCardCount, serial_port); // card_reader.c
        // pCardRead is reset to 0 by the function consuming the card number (See addRemoveAccess() in 'card_management.c')

        // TEMP FIX TO RESET CARD
        portableSleep(300); // Sleep for 1 second
        *actualArgs->pCardRead = 0;
    }

    serialDisconnect(serial_port); // Close the serial port connection
    return NULL; // No need to return a value, NULL is used when the return value is not used
}

void *runAdminConsol(void *args) {
    ThreadArgs *actualArgs = (ThreadArgs *)args;

    // Wait for the card reader thread to initialize the serial port
    pthread_mutex_lock(&mutex);
    while (!isCardReaderReady) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    int menu = systemMenu(actualArgs->pAccessCards, actualArgs->pCardsMallocated, actualArgs->pCardCount, actualArgs->pCardRead); // admin_menu.c

    if (menu == 0) {
        actualArgs->runCardReaderThread = false;
        printf("\033[33m* Shutting down system...\033[0m\n");
    } else {
        printf("\033[31m* System did not shut down correctly!\033[0m\n");
    }

    return NULL; // Return a null pointer since no value needs to be returned
}
