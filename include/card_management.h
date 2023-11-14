#ifndef CARD_MANAGEMENT_H
#define CARD_MANAGEMENT_H

#include <time.h>
#include <stdbool.h>

// Should these be in their own header file?
typedef struct {
    unsigned int cardNumber;
    int cardAccess;
    time_t dateCreated;
} accessCard;

enum cardAccess {
    NO_ACCESS, // First element == 0
    ACCESS
};

#define CARD_ID_LENGTH 12 // 8 chars + 3 spaces + \0 = 12 chars

void listAllCards(accessCard *pAccessCards, size_t *pCardCount);
void addRemoveAccess(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned int *pCardRead);

void addNewCard(accessCard **pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, int cardIndex, unsigned int cardNumber);
void updateCard(accessCard *pAccessCards, size_t *pCardCount, size_t card);
void setCardAccess(accessCard *pAccessCards, size_t cardIndex);
void removeCard(accessCard **pAccessCards, size_t *pCardCount, size_t cardIndex);

bool isValidRFIDFormat(char *rfid); // validate user input

#endif
