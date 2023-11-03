#include <stdio.h>
#include <stdbool.h>
#include <string.h> // strcmp
#include <stdlib.h> // Free memory in heap created by malloc
#include "safeinput.h"

#include "admin_menu.h"
#include "card_reader.h" // MCU RFID card reader
#include "card_management.h" // Struct for access cards
#include "data_storage.h" // Retrieve access cards from file

enum choice{
    SHUTDOWN_SYSTEM = 0,
    ADMIN_MENU = 1
};

int main(void) {
    // TODO: Load access cards from file into memory
    // TODO: Create struct for access cards
    size_t cardCount;
    size_t *pCardCount = &cardCount;
    accessCard *pAccessCards = retrieveAccessCards(&cardCount);

    // TODO: Run 2nd thread to have MCU access card database and validate RFID cards while admin use system.
    // while (true) {
    //     rfidReading();
    // }
    
    printf("DOOR ACCESS CONTROL SYSTEM RUNNING...\n");
    
    int choice = 1;

    do {
        GetInputInt("Enter command (0 = SHUTDOWN SYSTEM, 1 = ADMIN MENU): ", &choice);

        char adminPw[6] = "admin"; // TODO: Replace with hashed password
        char inputPw[21];

        int saveCardsResult;

        switch (choice) {
            case SHUTDOWN_SYSTEM:
                // End thread scanning for RFID cards - rfidReading();
                saveCardsResult = saveAccessCards(pAccessCards, cardCount);

                // Check if save to file was successful, else prompt user to try again or shutdown without saving.
                if (saveCardsResult == 0) {
                    printf("Cards saved successfully.\n");
                    free(pAccessCards);
                    printf("Shutting down DOOR ACCESS CONTROL SYSTEM...\n");
                    break;
                } else {
                    int shutdownChoice;
                    GetInputInt("Error saving cards. Enter 0 to cancel or 1 to shutdown system without saving: ", &shutdownChoice);
                    
                    if (choice == 0) {
                        break;
                    } else {
                        free(pAccessCards);
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
    
    // DO NOT FORGET TO free(pAccessCards); !!!!!!!!!!!!!!!
    return 0;
}
