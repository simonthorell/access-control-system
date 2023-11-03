#ifndef CARD_MANAGEMENT_H
#define CARD_MANAGEMENT_H

#include <time.h>

typedef struct {
    int cardNumber;
    int cardAccess;
    time_t dateCreated;
} accessCard;

enum cardAccess {
    NO_ACCESS, // First element == 0
    ACCESS
};

void listAllCards(accessCard *pAccessCards, size_t cardCount);
void addRemoveAccess(void);

#endif
