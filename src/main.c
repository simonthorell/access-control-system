#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "admin_menu.h"
#include "safeinput.h"

enum choice{
    SHUTDOWN_SYSTEM = 0,
    ADMIN_MENU = 1
};

int main(void) {
    // TODO: Load cards from file
    // TODO: Run thread to have MCU access card database
    
    printf("DOOR ACCESS CONTROL SYSTEM RUNNING...\n");
    int choice = 1;

    do {
        GetInputInt("Press 0 to SHUTDOWN SYSTEM and 1 to enter ADMIN MENU.\n", &choice);

        char adminPw[6] = "admin"; // REPLACE WITH ENCRYPTED PASSWORD
        char inputPw[21];

        switch (choice) {
            case SHUTDOWN_SYSTEM:
                printf("Shutting down door access control system...\n");
                break;
            case ADMIN_MENU:
                GetInput("Enter admin password: ", inputPw, 20);
                bool validPassword = strcmp(adminPw, inputPw) == 0;
                if (validPassword) {
                    adminMenu();
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
