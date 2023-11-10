#include <stdio.h>           // printf
#include <stdbool.h>         // bool
#include <string.h>          // strcmp (check pw)
#include <stdlib.h>          // free (memory deallocation)
#include <pthread.h>         // Threads

#include "safeinput.h"       // GetInputInt, GetInput
#include "admin_menu.h"      // Admin console UI menu
#include "card_reader.h"     // RFID card reader functions (RFID MCU connected via serial port)
#include "card_management.h" // Struct for access cards
#include "data_storage.h"    // Retrieve access cards from file & save to file (.csv)
#include "util_sleep.h"      // portableSleep (Windos/Linux/Mac compatible sleep function)
#include "connect_tcp_ip.h"  // Connect to door controller MCU wireless via TCP/IP

// IP address of wireless door microcontroller (ESP8266)
const char* ipAddressDoorController = "192.168.50.254"; // Make sure to use port 23 (defined in connect_wifi.h)

typedef struct {
    size_t *pCardsMallocated;
    size_t *pCardCount;
    accessCard *pAccessCards;
    volatile bool keepRunning;
    unsigned int *pCardRead; // Last read card number from MCU RFID card reader
} ThreadArgs;

enum choice{
    SHUTDOWN_SYSTEM = 0,
    ADMIN_MENU = 1
};

void startThreads(ThreadArgs *args); // Start multithreading with 2 threads.
void *runCardReader(void *args);     // Thread 1: MCU Card reader (Arduino/ESP32)
void *runAdminConsol(void *args);    // Thread 2: Admin console UI (Linux/Windows/Mac)

int main(void) {
    printf("Starting Door Access Control System...\n");

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
        fprintf(stderr, "Failed to load access cards. Exiting program...\n");
        return 1;
    } else {
        printf("Loaded %zu access cards from file 'access_cards.csv' into memory location: %p\n", cardCount, (void *)pAccessCards);
    }

    // Establish TCP/IP connection to door controller - connect_wifi.c
    while (sock == -1) {
        int connected = establishConnection(ipAddressDoorController);
        if (connected != -1) {
            printf("Connected (return code %d) to wireless door controller (ESP8266EX MCU) and established TCP/IP socket: %d\n", connected, sock);
            break;
        } else {
            printf("Failed to connect to door controller. Retrying...\n");
        }
        portableSleep(500); // Wait for 1 second before trying again
    }

    // Run program in two threads:
    // 1. MCU Card reader
    // 2. Admin console UI
    ThreadArgs args = {pCardsMallocated, pCardCount, pAccessCards, true, pCardRead};
    startThreads(&args);

    // close TCP/IP connection to door controller
    closeConnection();

    // Free memory allocated by retrieveAccessCards() after the threads have finished.
    free(pAccessCards); 
    printf("Memory deallocated successfully!\n");

    return 0;
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
    printf("Card reader running...\n");
    
    // if (sock = -1;) {
    //     establishConnection();
    // }

    // Run the MCU card reader until the admin console shuts down the system.
    while (actualArgs->keepRunning) {
        *actualArgs->pCardRead = rfidReading(actualArgs->pAccessCards, actualArgs->pCardCount); // card_reader.c
        portableSleep(500); // Sleep for a short duration to prevent this loop from consuming too much CPU.
        *actualArgs->pCardRead = 0; // Reset cardRead to 0 after reading card
    }

    printf("Terminating card reader...\n");
    return NULL; // No need to return a value, NULL is used when the return value is not used
}

void *runAdminConsol(void *args) {
    ThreadArgs *actualArgs = (ThreadArgs *)args;
    printf("Admin console running...\n");

    // TODO: Create encrypt/decrypt functions and store password hash in file. 
    char adminPw[6] = "admin";  
    
    int choice = 1;
    do {
        GetInputInt("Enter command (0 = SHUTDOWN SYSTEM, 1 = ADMIN MENU): ", &choice);
        char inputPw[21];

        switch (choice) {
            case SHUTDOWN_SYSTEM:
                // Save the cards and determine whether to shut down
                if (!saveAccessCards(actualArgs->pAccessCards, *(actualArgs->pCardCount))) {
                    printf("Cards saved successfully.\nShutting down Door Access Control System...\n");
                } else {
                    GetInputInt("Error saving cards. Enter 0 to shutdown system without saving or 1 to cancel: ", &choice);
                    if (!choice) {
                        printf("Shutting down DOOR ACCESS CONTROL SYSTEM without saving...\n");
                    } else {
                        printf("System will not shutdown. Returning to main menu...\n");
                        continue;
                    }
                }
                actualArgs->keepRunning = false;  // Terminate card reader
                break;
            case ADMIN_MENU:
                GetInput("Enter admin password: ", inputPw, 20);
                if (strcmp(adminPw, inputPw) == 0) {
                    adminMenu(actualArgs->pAccessCards, actualArgs->pCardsMallocated, actualArgs->pCardCount, actualArgs->pCardRead); // admin_menu.c
                } else {
                    printf("Invalid password!\n");
                }
                break;
            default:
                printf("Invalid choice! Try again!\n");
                break;
        }
    } while (choice != SHUTDOWN_SYSTEM);

    return NULL; // Return a null pointer since no value needs to be returned
}
