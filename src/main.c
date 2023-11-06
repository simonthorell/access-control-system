#include <stdio.h>
#include <stdbool.h>
#include <string.h> // strcmp
#include <stdlib.h> // Free memory in heap created by malloc
#include <pthread.h> // Threads
#include "safeinput.h"

#include "admin_menu.h"
#include "card_reader.h" // MCU RFID card reader
#include "card_management.h" // Struct for access cards
#include "data_storage.h" // Retrieve access cards from file & save to file

typedef struct {
    accessCard *pAccessCards;
    size_t *pCardCount;
} ThreadArgs;

enum choice{
    SHUTDOWN_SYSTEM = 0,
    ADMIN_MENU = 1
};

void startThreads(ThreadArgs *args);
void *runCardReader(void *args);
void *runAdminConsol(void *args);


int main(void) {
    // Load access cards from file into memory
    size_t cardCount, *pCardCount = &cardCount; // retrieveAccessCards() will count lines & update cardCount
    accessCard *pAccessCards = retrieveAccessCards(&cardCount); // Do not forget to free memory

    // Check if access cards were loaded successfully.
    if (pAccessCards == NULL) {
        fprintf(stderr, "Failed to load access cards. Exiting program...\n");
        return 1;
    } else {
        printf("Loaded %zu access cards from file 'access_cards.csv' into memory location: %p\n", cardCount, (void *)pAccessCards);
    }
 
    // Start the threads
    ThreadArgs args = {pAccessCards, pCardCount};
    startThreads(&args);

    free(pAccessCards); // Free memory allocated by retrieveAccessCards()
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
    printf("CARD READER RUNNING...\n");

    // Call the RFID reading function, passing the appropriate members from actualArgs
    rfidReading(actualArgs->pAccessCards, actualArgs->pCardCount);

    // Return a null pointer since no value needs to be returned
    return NULL;
}

void *runAdminConsol(void *args) {
    ThreadArgs *actualArgs = (ThreadArgs *)args;
    printf("ADMIN CONSOL RUNNING...\n");

    int choice = 1;
    char adminPw[6] = "admin";  // TODO: Create encrypt/decrypt functions and store password hash in file. 
    char inputPw[21];
    int saveCardsResult;

    do {
        GetInputInt("Enter command (0 = SHUTDOWN SYSTEM, 1 = ADMIN MENU): ", &choice);
        switch (choice) {
            case SHUTDOWN_SYSTEM:
                // Save the cards and determine whether to shut down
                saveCardsResult = saveAccessCards(actualArgs->pAccessCards, *(actualArgs->pCardCount));
                if (!saveCardsResult) {
                    printf("Cards saved successfully.\nShutting down DOOR ACCESS CONTROL SYSTEM...\n");
                    choice = SHUTDOWN_SYSTEM;  // Set choice to shutdown the loop
                } else {
                    int shutdownChoice;
                    GetInputInt("Error saving cards. Enter 0 to shutdown system without saving or 1 to cancel: ", &shutdownChoice);
                    if (!shutdownChoice) {
                        printf("Shutting down DOOR ACCESS CONTROL SYSTEM without saving...\n");
                        choice = SHUTDOWN_SYSTEM;  // Set choice to shutdown the loop
                    }
                }
                break;
            case ADMIN_MENU:
                GetInput("Enter admin password: ", inputPw, 20);
                if (strcmp(adminPw, inputPw) == 0) {
                    adminMenu(actualArgs->pAccessCards, actualArgs->pCardCount);
                } else {
                    printf("Invalid password!\n");
                }
                break;
            default:
                printf("Invalid choice! Try again!\n");
                break;
        }
    } while (choice != SHUTDOWN_SYSTEM);

    // Return a null pointer since no value needs to be returned
    return NULL;
}
