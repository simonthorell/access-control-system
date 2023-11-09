#include <stdio.h>
#include "safeinput.h"

#include "door_control.h"
#include "card_management.h"
#include "card_reader.h"

#include "admin_menu.h"

int adminMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned int *pCardRead) {
    printMenuOptions();
    
    int menuChoice;

    do {
        menuChoice = getMenuChoice();

        switch (menuChoice) {
            case SHOW_MENU:
                printMenuOptions();
                break;
            case REMOTE_OPEN_DOOR:
                remoteOpenDoor();
                break;
            case LIST_CARDS:
                listAllCards(pAccessCards, pCardCount);
                break;
            case ADD_REMOVE_ACCESS:
                addRemoveAccess(pAccessCards, pCardsMallocated, pCardCount, pCardRead);
                break;
            case EXIT:
                printf("Exiting admin menu...\n");
                break;
            case FAKE_TEST_SCAN_CARD:
                fakeTestScanCard(pAccessCards, pCardCount);
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
    GetInputInt("Enter your choice (enter 0 to show options): ", &choice);
    return choice;
}
