#ifndef ADMIN_MENU_H
#define ADMIN_MENU_H

#include "card_management.h" // accessCard struct
#include "data_storage.h"    // Configuration struct

typedef struct {
    int key;
    const char *value;
} MenuOption;

// TODO: move
int promptAdminPassword(void);

int systemMenu(accessCard *pAccessCards, Configuration *pConfig, size_t *pCardsMallocated, size_t *pCardCount, unsigned long int *pCardRead);
void adminMenu(accessCard *pAccessCards, Configuration *pConfig, size_t *pCardsMallocated, size_t *pCardCount, unsigned long int *pCardRead);
int scanCardSubMenu(void);
int addNewCardSubMenu(void);
void updateCardSubMenu(accessCard *pAccessCards, size_t *pCardCount, size_t cardIndex);
int updateAccessMenu(void);
void systemConfigMenu(Configuration *pConfig);

#endif
