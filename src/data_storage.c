#include "data_storage.h"
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <time.h>

void saveAccessCards(accessCard *pAccessCards, size_t cardCount) {
    // Code to save access cards from heap to file. 
    printf("Saving %zu access cards from memory location: %p to file 'access_cards.csv'...\n", cardCount, (void *)pAccessCards);
}

accessCard* retrieveAccessCards(size_t *cardCount) {
    FILE *file = fopen("access_cards.csv", "r");
    if (!file) return NULL;

    char line[256]; // buffer for reading a line from the .csv file. Make smaller?? 
    *cardCount = 0;
    size_t capacity = 10;

    // Allocate space for the array in the heap
    accessCard *accessCards = malloc(capacity * sizeof(accessCard));

    // Skip header line
    fgets(line, sizeof(line), file);

    // Check that accessCards is not NULL && check that successfully read a line from the file.
    while (accessCards && fgets(line, sizeof(line), file)) {
        // CHeck if we need to resize the array, if so DOUBLE the capacity. 
        if (*cardCount >= capacity) {
            capacity *= 2;
            accessCard *temp = realloc(accessCards, capacity * sizeof(accessCard));
            if (!temp) break;
            accessCards = temp;
        }

        // Temp variables to store the parsed values
        char statusStr[20];
        int cardNumber;
        time_t dateCreated;

        // Read the CSV line and parse it
        if (sscanf(line, "%d,%19[^,],%ld", &cardNumber, statusStr, &dateCreated) == 3) {
            accessCards[*cardCount].cardNumber = cardNumber;
            accessCards[*cardCount].cardAccess = strcmp(statusStr, "ACCESS") == 0 ? ACCESS : NO_ACCESS;
            accessCards[*cardCount].dateCreated = dateCreated;
            (*cardCount)++;
        }
    }

    fclose(file);
    return accessCards; // Caller is responsible for freeing the memory!!!
}

// accessCard* retrieveAccessCards(size_t *cardCount) {
//     printf("Retrieving saved access cards from file...\n");

//     // ONLY FOR TESTING (Static array - persists after the function returns)
//     // Replace with readFromFile() function
//     static accessCard accessCards[] = {
//         {1000, ACCESS, 1698166400},
//         {1001, NO_ACCESS, 1698166400},
//         {1002, ACCESS, 1698166400}
//     };

//     // Set the card count
//     if (cardCount) {
//         *cardCount = sizeof(accessCards) / sizeof(accessCards[0]);
//     }

//     return accessCards;
// }
