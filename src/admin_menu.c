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
    printf("\n\033[1;90m******** ADMIN MENU ********\033[0m\n");
    printf("1. Remote open door\n");
    printf("2. List all cards in system\n");
    printf("3. Add/update/remove cards\n");
    printf("4. Exit admin menu\n");
    printf("9. Test scan fake card\n");
    printf("\033[1;90m****************************\033[0m\n");
}

int getMenuChoice(void){
    int choice;
    GetInputInt("\n\033[4mEnter your option (0 = options)\033[0m: ", &choice);
    return choice;
}
