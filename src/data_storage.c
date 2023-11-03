#include "data_storage.h"
#include <stdio.h>

void saveAccessCards(void) {
    // time_t dateNow = time(NULL); // Generate current date/time
    // Code to save configuration to file. 
    printf("Saving configuration settings...\n");
}

accessCard* retrieveAccessCards(size_t *cardCount) {
    printf("Retrieving saved cards...\n");

    // Static array - persists after the function returns
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
