#include <stdio.h>
#include <stdlib.h>

#include "safeinput.h"
#include "door_control.h"
#include "card_management.h"
#include "card_reader.h"
#include "admin_menu.h"

// User input
int getMenuChoice(int menuOptions[], size_t menuOptionsSize){
    while (1) {
        int choice;
        // Validate that GetInputInt returns true and that the choice is in the menuOptions array
        if (GetInputInt("\n\033[4mEnter your option (0 = options)\033[0m: ", &choice)) {
            for (size_t i = 0; i < menuOptionsSize; i++) {
                if (choice == menuOptions[i]) {
                    return choice;
                    break;
                }
            }
        }
        printf("Invalid choice! Try Again! \n");
    }
    return EXIT_FAILURE;
}

int getCardNumber(char* cardNumberInput, int cardIdLength){
    while (true) {
        GetInput("\n\033[4mEnter card RFID\033[0m: ", cardNumberInput, cardIdLength);
        
        if (isValidRFIDFormat(cardNumberInput)) {
            break;
            return EXIT_SUCCESS;
        } else {
            printf("Invalid RFID format! Try Again! \n");
        }
    }
    return EXIT_FAILURE;
}

// Main menu for admin
void printAdminMenu(void){
    printf("\n\033[1;90m******** ADMIN MENU ********\033[0m\n");
    printf("1. Remote open door\n");
    printf("2. List all cards in system\n");
    printf("3. Add/update/remove cards\n");
    printf("4. Exit admin menu\n");
    printf("9. Test scan fake card\n");
    printf("\033[1;90m****************************\033[0m\n");
}

int adminMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned int *pCardRead) {
    printAdminMenu();
    
    int menuChoice;
    int menuOptions[] = {SHOW_MENU, REMOTE_OPEN_DOOR, LIST_CARDS, ADD_REMOVE_ACCESS, EXIT, FAKE_TEST_SCAN_CARD};
    size_t menuOptionsSize = sizeof(menuOptions) / sizeof(menuOptions[0]);

    do {
        menuChoice = getMenuChoice(menuOptions, menuOptionsSize);

        switch (menuChoice) {
            case SHOW_MENU:
                printAdminMenu();
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

void printScanCardSubMenu(void) {
    printf("\n\033[1;90m******** CARD MENU ********\033[0m\n");
    printf("1. Scan RFID card\n");
    printf("2. Enter card ID manually\n");
    printf("3. Back\n");
    printf("\033[1;90m****************************\033[0m\n");
}

int ScanCardSubMenu(void) {
    printScanCardSubMenu();

    int menuOptions[] = {1, 2, 3};
    size_t menuOptionsSize = sizeof(menuOptions) / sizeof(menuOptions[0]);
    int choice = getMenuChoice(menuOptions, menuOptionsSize);
    return choice;
}

void printAddNewCardSubMenu(char *cardNumberString) {
    printf("Card with RFID '%s' not found. Do you want to add a new card?\n", cardNumberString);
    printf("1. Yes\n");
    printf("2. No (back)\n");
}

int addNewCardSubMenu(char *cardNumberString) {
    printAddNewCardSubMenu(cardNumberString);

    int menuOptions[] = {1, 2};
    size_t menuOptionsSize = sizeof(menuOptions) / sizeof(menuOptions[0]);
    int choice = getMenuChoice(menuOptions, menuOptionsSize);

    return choice;
}

void printUpdateCardSubMenu(void) {
    printf("\n\033[1;90m******** UPDATE CARD MENU ********\033[0m\n");
    printf("1. Change access\n");
    printf("2. Remove card\n");
    printf("3. Back\n");
    printf("\033[1;90m**********************************\033[0m\n");
}

int updateCardSubMenu(void) {
    printUpdateCardSubMenu();

    int menuOptions[] = {1, 2, 3};
    size_t menuOptionsSize = sizeof(menuOptions) / sizeof(menuOptions[0]);
    int choice = getMenuChoice(menuOptions, menuOptionsSize);

    return choice;
}
