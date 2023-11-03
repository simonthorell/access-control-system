#include "admin_menu.h"
#include <stdio.h>
#include "safeinput.h"

#include "door_control.h"
#include "card_management.h"
#include "card_reader.h"

int adminMenu(void) {
    printMenuOptions();
    
    int menuChoice;

    do {
        menuChoice = getMenuChoice();

        switch (menuChoice) {
            case REMOTE_OPEN_DOOR:
                remoteOpenDoor();
                break;
            case LIST_CARDS:
                listAllCards();
                break;
            case ADD_REMOVE_ACCESS:
                printf("CREATE SUBMENU HERE!\n");
                addRemoveAccess();
                break;
            case EXIT:
                // free memory
                printf("Exiting admin menu...\n");
                break;
            case FAKE_TEST_SCAN_CARD:
                fakeTestScanCard();
                break;
            default:
                printf("Invalid choice! Try Again! \n");
                break;
        }
    } while (menuChoice != EXIT);

    return 0;
}

void printMenuOptions(void){
    printf("1. Remote open door\n");
    printf("2. List all cards in system\n");
    printf("3. Add/remove access\n");
    printf("4. Exit\n");
    printf("9. FAKE TEST SCAN CARD\n");
}

int getMenuChoice(void){
    int choice;
    GetInputInt("Enter your choice: ", &choice);
    return choice;
}
