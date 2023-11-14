#ifndef ADMIN_MENU_H
#define ADMIN_MENU_H

#include "card_management.h"

enum menuChoice {
    SHOW_MENU = 0,
    REMOTE_OPEN_DOOR = 1,
    LIST_CARDS = 2,
    ADD_REMOVE_ACCESS = 3,
    EXIT = 4,
    FAKE_TEST_SCAN_CARD = 9
};

// user inputs
int getMenuChoice(int menuOptions[], size_t menuOptionsSize);
int getCardNumber(char* cardNumberInput, int cardIdLength);

// admin main menu
void printAdminMenu(void);
int adminMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned int *pCardRead);

// card management submenus
void printScanCardSubMenu(void);
int ScanCardSubMenu(void);

void printAddNewCardSubMenu(char *uintToHex);
int addNewCardSubMenu(char *uintToHex);

void printUpdateCardSubMenu(void);
int updateCardSubMenu(void);

#endif
