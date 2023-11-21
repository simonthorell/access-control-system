#include "input_output.h"       // print, get user inputs
#include "door_control.h"       // remoteOpenDoor
#include "card_management.h"    // listAllCards, addRemoveAccess
#include "card_reader.h"        // fakeTestScanCard
#include "admin_menu.h"         // MenuOptions struct
#include "encrypt_decrypt.h"    // encryptPassword, decryptPassword

int systemMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned long int *pCardRead) {
    char *headerTitle = "SYSTEM MENU";
    int headerLength = 19;
    MenuOption options[] = {
        {1, "Admin menu"},
        {2, "Shutdown System"}
    };
    size_t optionsSize = sizeof(options) / sizeof(options[0]);
    
    while (1) {
        printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c

        int choice = getMenuChoice(options, optionsSize);
        switch (choice) {
            case 0:
                printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c
                break;
            case 1:
                if (promptAdminPassword()) {
                    adminMenu(pAccessCards, pCardsMallocated, pCardCount, pCardRead);
                }
                break;
            case 2:
                return 0; // Exit program
        }
    }
    return 1; // Program not shutdown correctly - pass error warning to main
}

void adminMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned long int *pCardRead) {
    char *headerTitle = "ADMIN MENU";
    int headerLength = 27;
    MenuOption options[] = {
        {1, "Remote open door"},
        {2, "List all cards in system"},
        {3, "Add/update/remove cards"},
        {4, "Exit admin menu"},
        {8, "Change password"},
        {9, "Test scan fake card"}
    };
    size_t optionsSize = sizeof(options) / sizeof(options[0]);

    while (1) {
        printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c
        int choice = getMenuChoice(options, optionsSize);
        switch (choice) {
            case 0:
                // Only to let user display options again!
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
            case 8:
                changeAdminPassword();
                break;
            case 9:
                fakeTestScanCard(pAccessCards, pCardCount);
                break;
        }
    } 
}

int scanCardSubMenu(void) {
    char *headerTitle = "SCAN CARD";
    int headerLength = 25;
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
    char *headerTitle = "NEW CARD";
    int headerLength = 16;
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
    char *headerTitle = "UPDATE CARD";
    int headerLength = 20;
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

int updateAccessMenu(void) {
    char *headerTitle = "SET ACCESS";
    int headerLength = 18;
    MenuOption options[] = {
        {1, "ACCESS"},
        {2, "NO ACCESS"}
    };
    size_t optionsSize = sizeof(options) / sizeof(options[0]);
    printMenu(headerTitle, headerLength, options, optionsSize); // input_output.c

    int choice = getMenuChoice(options, optionsSize); // input_output.c
    switch (choice) {
        case 1:
            return ACCESS;
            break;
        case 2:
            return NO_ACCESS;
            break;
    }
    return -1; // Function did not return a valid value
}
