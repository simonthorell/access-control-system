#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "safeinput.h"

#include "admin_menu.h"
#include "card_reader.h"
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
    accessCard *pAccessCards = retrieveAccessCards(&cardCount);

    // TODO: Run 2nd thread to have MCU access card database and validate RFID cards while admin use system.
    // while (true) {
    //     rfidReading();
    // }
    
    printf("DOOR ACCESS CONTROL SYSTEM RUNNING...\n");
    
    int choice = 1;

    do {
        GetInputInt("Press 0 to SHUTDOWN SYSTEM and 1 to enter ADMIN MENU.\n", &choice);

        // TODO: Replace with hashed password
        char adminPw[6] = "admin"; 
        char inputPw[21];

        switch (choice) {
            case SHUTDOWN_SYSTEM:
                // End thread scanning for RFID cards - rfidReading();
                saveAccessCards(pAccessCards, cardCount);
                printf("Shutting down door access control system...\n");
                break;
            case ADMIN_MENU:
                GetInput("Enter admin password: ", inputPw, 20);
                bool validPassword = strcmp(adminPw, inputPw) == 0;
                if (validPassword) {
                    adminMenu(pAccessCards, cardCount);
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
