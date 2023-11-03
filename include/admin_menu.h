#ifndef ADMIN_MENU_H
#define ADMIN_MENU_H

enum menuChoice {
    REMOTE_OPEN_DOOR = 1,
    LIST_CARDS = 2,
    ADD_REMOVE_ACCESS = 3,
    EXIT = 4,
    FAKE_TEST_SCAN_CARD = 9
};

int adminMenu(void);
void printMenuOptions(void);
int getMenuChoice(void);

#endif
