#ifndef ADMIN_MENU_H
#define ADMIN_MENU_H

#include "card_management.h" // accessCard struct

typedef struct {
    int key;
    const char *value;
} MenuOption;

void adminMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned int *pCardRead);
int scanCardSubMenu(void);
int addNewCardSubMenu(void);
void updateCardSubMenu(accessCard *pAccessCards, size_t *pCardCount, size_t cardIndex);

#endif
