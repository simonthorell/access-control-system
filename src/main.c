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
#include "status_messages.h" // Custom error/success handling
#include "input_output.h"    // Custom input/output functions (printHeader, printMessage etc.)

typedef struct {
    size_t *pCardsMallocated;
    size_t *pCardCount;
    accessCard *pAccessCards;
    Configuration *pConfig;             // Pointer to Configuration struct
    unsigned long int *pCardRead;       // Last read card number from MCU RFID card reader
    volatile bool *pRunCardReaderThread;
} ThreadArgs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool isCardReaderReady = false;

void startThreads(ThreadArgs *args);   // Start multithreading with 2 threads.
void *runCardReader(void *args);       // Thread 1: MCU Card reader (Arduino/ESP32)
void *runAdminConsol(void *args);      // Thread 2: Admin console UI (Linux/Windows/Mac)

int main(void) {
    printMenuHeader("DOOR ACCESS CONTROL SYSTEM", 34);

    // Load access cards from file into memory (heap)
    size_t cardsMallocated = 10;    // Initial value to alloc for amount of cards
    size_t *pCardsMallocated = &cardsMallocated;
    size_t cardCount = 0;           // retrieveAccessCards() will count lines & update cardCount
    size_t *pCardCount = &cardCount; 
    accessCard *pAccessCards = retrieveAccessCards(&cardsMallocated, &cardCount); // Do not forget to free memory
    volatile bool runCardReaderThread = true;
    volatile bool *pRunCardReaderThread = &runCardReaderThread;

    unsigned long int cardRead = 0;           // Card number read by MCU RFID card reader
    unsigned long int *pCardRead = &cardRead; // Pointer to last card read by MCU RFID card reader.

    // Check if access cards were loaded successfully from file to heap.
    if (pAccessCards == NULL) {
        printStatusMessage(ERROR_FILE_NOT_FOUND, "Failed to load access cards. Exiting program...");
        return EXIT_FAILURE; // Exit program
    } else {
        printStatusMessage(SUCCESS, "Loaded %zu access cards from file 'access_cards.csv' into memory location: %p", cardCount, (void *)pAccessCards);
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
    ThreadArgs args = {pCardsMallocated, pCardCount, pAccessCards, pConfig, pCardRead, pRunCardReaderThread};
    startThreads(&args);

    // close TCP/IP connection
    closeConnection();

    // save access cards to file
    int cardsSaved = saveAccessCards(pAccessCards, *pCardCount); // data_storage.c
    if (cardsSaved == SUCCESS){ 
        printStatusMessage(SUCCESS, "Saved %zu access cards to file 'access_cards.csv'", *pCardCount);
    } else {
        printStatusMessage(ERROR_FILE_NOT_FOUND, "Failed to save access cards to file 'access_cards.csv'");
    }

    // Clean up memory
    free(pAccessCards);  // Free memory allocated by retrieveAccessCards() in data_storage.c
    free(pConfig);       // Free memory allocated by readConfig() in data_storage.c
    printStatusMessage(SUCCESS, "Memory deallocated successfully");
    printMenuHeader("PROGRAM EXITED SUCCESSFULLY", 36); // "DOOR ACCESS CONTROL SYSTEM

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
    if (serial_port == ERROR_OPERATION_FAILED) {
        actualArgs->pRunCardReaderThread = false;
        isCardReaderReady = true;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        return NULL;
    } else {
        isCardReaderReady = true;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    // Run the MCU card reader until the admin console request to shut down the system.
    while (*actualArgs->pRunCardReaderThread) {
        // This will re-loop everytime a card has been read by the MCU RFID card reader.
        *actualArgs->pCardRead = rfidReading(actualArgs->pRunCardReaderThread, actualArgs->pAccessCards, actualArgs->pCardCount, serial_port); // card_reader.c
        // pCardRead is reset to 0 by the function consuming the card number (See addRemoveAccess() in 'card_management.c')

        // TODO: Could the be achieved with a mutex instead of a sleep?
        portableSleep(1000); // Sleep for 1 second
        *actualArgs->pCardRead = 0;
    }

    serialDisconnect(serial_port); // Close the serial port connection
    return NULL; // No need to return a value, NULL is used when the return value is not used (required for POSIX threads)
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
        actualArgs->pRunCardReaderThread = false;
        printInfoMessage("Shutting down system...");
    }

    return NULL; // Return a null pointer since no value needs to be returned (required for POSIX threads)
}
