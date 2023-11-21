#ifndef ADMIN_MENU_H
#define ADMIN_MENU_H

#include "card_management.h" // accessCard struct

typedef struct {
    int key;
    const char *value;
} MenuOption;

// TODO: move
int promptAdminPassword(void);

int systemMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned long int *pCardRead);
void adminMenu(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned long int *pCardRead);
int scanCardSubMenu(void);
int addNewCardSubMenu(void);
void updateCardSubMenu(accessCard *pAccessCards, size_t *pCardCount, size_t cardIndex);
int updateAccessMenu(void);

#endif
