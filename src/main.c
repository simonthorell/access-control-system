#include <stdio.h>           // printf
#include <stdbool.h>         // bool
#include <string.h>          // strcmp
#include <stdlib.h>          // Free memory in heap created by malloc
#include <pthread.h>         // Threads
#include "safeinput.h"       // GetInputInt, GetInput

#include "admin_menu.h"      // Admin menu
#include "card_reader.h"     // MCU RFID card reader
#include "card_management.h" // Struct for access cards
#include "data_storage.h"    // Retrieve access cards from file & save to file
#include "util_sleep.h"      // Sleep function for Windows, Mac & Linux

typedef struct {
    size_t *pCardsMallocated;
    size_t *pCardCount;
    accessCard *pAccessCards;
    volatile bool keepRunning;
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
    size_t cardsMallocated, *pCardsMallocated = &cardsMallocated; // Amount of memory currently allocated for access cards.
    size_t cardCount, *pCardCount = &cardCount; // retrieveAccessCards() will count lines & update cardCount
    accessCard *pAccessCards = retrieveAccessCards(&cardsMallocated, &cardCount); // Do not forget to free memory

    // Check if access cards were loaded successfully from file to heap.
    if (pAccessCards == NULL) {
        fprintf(stderr, "Failed to load access cards. Exiting program...\n");
        return 1;
    } else {
        printf("Loaded %zu access cards from file 'access_cards.csv' into memory location: %p\n", cardCount, (void *)pAccessCards);
    }
 
    // Run Threads - 1. MCU Card reader, 2. Admin console UI
    ThreadArgs args = {pCardsMallocated, pCardCount, pAccessCards, true};
    startThreads(&args);

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

    // Run the MCU card reader until the admin console shuts down the system.
    while (actualArgs->keepRunning) {
        portableSleep(1); // Sleep for a short duration to prevent this loop from consuming too much CPU.
        rfidReading(actualArgs->pAccessCards, actualArgs->pCardCount); // card_reader.c
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
                    adminMenu(actualArgs->pAccessCards, actualArgs->pCardsMallocated, actualArgs->pCardCount); // admin_menu.c
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
