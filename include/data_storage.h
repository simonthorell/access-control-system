#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "card_management.h" // Access to card struct and access enum

void saveAccessCards(void);
accessCard* retrieveAccessCards(size_t *cardCount);

#endif
