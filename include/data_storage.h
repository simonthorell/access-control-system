#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "card_management.h" // Access to card struct and access enum

int saveAccessCards(accessCard *pAccessCards, size_t cardCount);
accessCard* retrieveAccessCards(size_t *cardsMallocated, size_t *cardCount);

#endif
