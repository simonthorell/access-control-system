#include "input_output.h"       // print, get user inputs
#include "door_control.h"       // remoteOpenDoor
#include "card_management.h"    // listAllCards, addRemoveAccess
#include "card_reader.h"        // fakeTestScanCard
#include "admin_menu.h"         // MenuOptions struct

void adminMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned int *pCardRead) {
    char *headerTitle = "ADMIN MENU";
    int headerLength = 28;
    MenuOption options[] = {
        {1, "Remote open door"},
        {2, "List all cards in system"},
        {3, "Add/update/remove cards"},
        {4, "Exit admin menu"},
        {9, "Test scan fake card"}
    };
    size_t optionsSize = sizeof(options) / sizeof(options[0]);
    printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c

    while (1) {
        int choice = getMenuChoice(options, optionsSize);
        switch (choice) {
            case 0:
                printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c
                break;
            case 1:
                remoteOpenDoor();
                break;
            case 2:
                listAllCards(pAccessCards, pCardCount);
                break;
            case 3:
                addRemoveAccess(pAccessCards, pCardsMallocated, pCardCount, pCardRead);
                break;
            case 4:
                return; // Back to system menu
            case 9:
                fakeTestScanCard(pAccessCards, pCardCount);
                break;
        }
    } 
}

int scanCardSubMenu(void) {
    char *headerTitle = "SCAN CARD MENU";
    int headerLength = 33;
    MenuOption options[] = {
        {1, "Scan RFID card"},
        {2, "Enter card ID manually"},
        {3, "Back"}
    };
    size_t optionsSize = sizeof(options) / sizeof(options[0]);
    printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c

    int choice = getMenuChoice(options, optionsSize); // input_output.c
    return choice; // Used to create new card in card_management.c
}

int addNewCardSubMenu(void) {
    char *headerTitle = "ADD NEW CARD";
    int headerLength = 28;
    MenuOption options[] = {
        {1, "Add New Card"},
        {2, "Back"}
    };
    size_t optionsSize = sizeof(options) / sizeof(options[0]);
    printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c

    int choice = getMenuChoice(options, optionsSize); // input_output.c
    return choice; // Used to create new card in card_management.c
}

void updateCardSubMenu(accessCard *pAccessCards, size_t *pCardCount, size_t cardIndex) {
    char *headerTitle = "UPDATE CARD MENU";
    int headerLength = 33;
    MenuOption options[] = {
        {1, "Change access"},
        {2, "Remove card"},
        {3, "Back"}
    };
    size_t optionsSize = sizeof(options) / sizeof(options[0]);
    printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c

    int choice = getMenuChoice(options, optionsSize); // input_output.c
    switch (choice) {
        case 1:
            setCardAccess(pAccessCards, cardIndex);
            break;
        case 2:
            removeCard(&pAccessCards, pCardCount, cardIndex);
            break;
        case 3:
            return; // Back to admin menu
    }
}
