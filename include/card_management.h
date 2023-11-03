#ifndef CARD_MANAGEMENT_H
#define CARD_MANAGEMENT_H

enum cardAccess {
    NO_ACCESS = 0,
    ACCESS = 1
};

void listAllCards(void);
void addRemoveAccess(void);

#endif
