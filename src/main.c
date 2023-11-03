#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "admin_menu.h"
#include "safeinput.h"

enum choice{
    EXIT_SYSTEM = 0,
    ADMIN_MENU = 1
};

int main(void) {
    // TODO: Load cards from file
    // TODO: Run thread to have MCU access card database
    
    printf("DOOR ACCESS CONTROL SYSTEM RUNNING...\n");
    int choice = 1;

    do {
        GetInputInt("Press 0 to EXIT SYSTEM and 1 to enter ADMIN MENU.\n", &choice);

        char adminPw[6] = "admin"; // REPLACE WITH ENCRYPTED PASSWORD
        char inputPw[21];

        switch (choice) {
            case EXIT_SYSTEM:
                printf("Exiting...\n");
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

    } while (choice != EXIT_SYSTEM);
    
    return 0;
}
