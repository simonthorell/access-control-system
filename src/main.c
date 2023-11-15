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
#include "util_sleep.h"      // portableSleep (Windos/Linux/Mac compatible sleep function)
#include "connect_tcp_ip.h"  // Connect to door controller MCU wireless via TCP/IP

typedef struct {
    size_t *pCardsMallocated;
    size_t *pCardCount;
    accessCard *pAccessCards;
    Configuration *pConfig;     // Pointer to Configuration struct
    unsigned int *pCardRead;    // Last read card number from MCU RFID card reader
    volatile bool keepRunning;  // TODO: Change to atomic bool
} ThreadArgs;

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

    unsigned int cardRead = 0;           // Card number read by MCU RFID card reader
    unsigned int *pCardRead = &cardRead; // Pointer to last card read by MCU RFID card reader.

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

    // Establish TCP/IP connection to door controller - connect_wifi.c
    while (sock == -1) {
        int connected = establishConnection(pConfig->door_ip_address);
        if (connected != -1) {
            printf("\033[32m* Connected to wireless door controller (ESP8266EX MCU) with return code %d and established TCP/IP socket: %d\033[0m\n", connected, sock);
            break;
        } else {
            printf("\033[31m* Failed to connect to door controller. Retrying...\033[0m\n");
        }
        portableSleep(500); // Wait for 1 second before trying again
    }

    // Start multithreading - 1. MCU Card reader, 2. Admin console UI
    ThreadArgs args = {pCardsMallocated, pCardCount, pAccessCards, pConfig, pCardRead, true};
    startThreads(&args);

    // close TCP/IP connection to door controller
    closeConnection();

    // Clean up memory
    free(pAccessCards);  // Free memory allocated by retrieveAccessCards() in data_storage.c
    free(pConfig);       // Free memory allocated by readConfig() in data_storage.c
    printf("\033[32m* Memory deallocated successfully!\033[0m\n");

    return EXIT_SUCCESS;
}

void startThreads(ThreadArgs *args) {
    pthread_t thread1, thread2;

    // Create the card reader thread
    if (pthread_create(&thread1, NULL, runCardReader, (void *)args)) {
        perror("Error creating card reader thread");
        exit(EXIT_FAILURE);
    }

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
    // pthread_mutex_lock(&actualArgs->mutex);

    // Connect to the RFID reader on the serial port
    int serial_port = open(actualArgs->pConfig->rfid_serial_port, O_RDWR);
    
    if (serial_port == -1) {
        printf("\033[31m* Could not connect to RFID reader on serial port '%s' \033[0m\033[33m(Check serial port settings in admin menu!)\033[0m\n", actualArgs->pConfig->rfid_serial_port);
        return NULL;
    } else {
        printf("\033[32m* Connected to RFID reader on serial port %s\033[0m\n", actualArgs->pConfig->rfid_serial_port);
    }

    // Run the MCU card reader until the admin console shuts down the system.
    while (actualArgs->keepRunning) {
        *actualArgs->pCardRead = rfidReading(actualArgs->pAccessCards, actualArgs->pCardCount, serial_port); // card_reader.c
        // TODO: Move this to function reading the card
        portableSleep(500); // Sleep for a short duration to prevent this loop from consuming too much CPU.
        *actualArgs->pCardRead = 0; // Reset cardRead to 0 after reading card
    }

    close(serial_port); // Remember to close the port

    printf("* Terminating card reader...\n");
    return NULL; // No need to return a value, NULL is used when the return value is not used
}

void *runAdminConsol(void *args) {
    ThreadArgs *actualArgs = (ThreadArgs *)args;

    portableSleep(500); // TODO: Temporary fix to wait for card reader to start up. Replace with mutex.
    int menu = systemMenu(actualArgs->pAccessCards, actualArgs->pCardsMallocated, actualArgs->pCardCount, actualArgs->pCardRead); // admin_menu.c

    if (menu == 0) {
        printf("* Shutting down DOOR ACCESS CONTROL SYSTEM...\n");
    } else {
        printf("* Terminating admin console...\n");
    }

    return NULL; // Return a null pointer since no value needs to be returned
}
