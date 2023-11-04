#include <stdio.h>
#include <stdbool.h>
#include <string.h> // strcmp
#include <stdlib.h> // Free memory in heap created by malloc
#include "safeinput.h"

#include "admin_menu.h"
#include "card_reader.h" // MCU RFID card reader
#include "card_management.h" // Struct for access cards
#include "data_storage.h" // Retrieve access cards from file & save to file

enum choice{
    SHUTDOWN_SYSTEM = 0,
    ADMIN_MENU = 1
};

int runCardReader(accessCard *pAccessCards, size_t *pCardCount);
int runAdminConsol(accessCard *pAccessCards, size_t *pCardCount);


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

    // TODO: Run card reader and admin console in separate threads
    while (true) {
        int runningCardReader = runCardReader(pAccessCards, pCardCount);
        int runningAdminConsol = runAdminConsol(pAccessCards, pCardCount);

        if (!runningCardReader && !runningAdminConsol) {
            free(pAccessCards); // Free memory allocated by retrieveAccessCards()
            printf("Memory deallocated successfully!\n");
            break;
        }
    }

    return 0;
}


int runCardReader(accessCard *pAccessCards, size_t *pCardCount) {
    printf("CARD READER RUNNING...\n");
    // TODO: Run 2nd thread to have MCU access card database and validate RFID cards while admin use system.
    // rfidReading(); // card_reader.c
    
    // TEMPORARY FOR TESTING
    (void)pAccessCards;
    (void)pCardCount;

    return 0;
}

int runAdminConsol(accessCard *pAccessCards, size_t *pCardCount) {
    printf("ADMIN CONSOL RUNNING...\n");
    
    int choice = 1;

    do {
        GetInputInt("Enter command (0 = SHUTDOWN SYSTEM, 1 = ADMIN MENU): ", &choice);

        char adminPw[6] = "admin"; // TODO: Replace with hashed password
        char inputPw[21];

        int saveCardsResult;

        switch (choice) {
            case SHUTDOWN_SYSTEM:
                // End thread scanning for RFID cards - rfidReading();
                saveCardsResult = saveAccessCards(pAccessCards, *pCardCount);

                // Check if save to file was successful, else prompt user to try again or shutdown without saving.
                if (!saveCardsResult) {
                    printf("Cards saved successfully.\n");
                    printf("Shutting down DOOR ACCESS CONTROL SYSTEM...\n");
                    break;
                } else {
                    int shutdownChoice;
                    GetInputInt("Error saving cards. Enter 0 to shutdown system without saving or 1 to cancel: ", &shutdownChoice);
                    if (shutdownChoice) {
                        break;
                    } else {
                        printf("Shutting down DOOR ACCESS CONTROL SYSTEM...\n");
                        break;
                    }
                }

            case ADMIN_MENU:
                GetInput("Enter admin password: ", inputPw, 20);
                bool validPassword = strcmp(adminPw, inputPw) == 0;

                if (validPassword) {
                    adminMenu(pAccessCards, pCardCount);
                } else {
                    printf("Invalid password!\n");
                }
                continue;
            default:
                printf("Invalid choice! Try again!\n");
                break;
        }

    } while (choice != SHUTDOWN_SYSTEM);

    return 0;
}
