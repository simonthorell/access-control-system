#include "data_storage.h"
#include <stdio.h>
#include <stdlib.h> // malloc, free

void saveAccessCards(accessCard *pAccessCards, size_t cardCount) {
    // Code to save access cards from heap to file. 
    printf("Saving %zu access cards from memory: %p to file...\n", cardCount, (void *)pAccessCards);
}

accessCard* retrieveAccessCards(size_t *cardCount) {
    printf("Retrieving saved access cards from file...\n");

    // ONLY FOR TESTING (Static array - persists after the function returns)
    // Replace with readFromFile() function
    static accessCard accessCards[] = {
        {1000, ACCESS, 1698166400},
        {1001, NO_ACCESS, 1698166400},
        {1002, ACCESS, 1698166400}
    };

    // Set the card count
    if (cardCount) {
        *cardCount = sizeof(accessCards) / sizeof(accessCards[0]);
    }

    return accessCards;
}
