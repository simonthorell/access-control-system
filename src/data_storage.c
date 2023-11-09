#include "data_storage.h"
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>
#include <time.h>

int saveAccessCards(accessCard *pAccessCards, size_t cardCount) {
    printf("Saving %zu access cards from memory location: %p to file 'access_cards.csv'...\n", cardCount, (void *)pAccessCards);

    FILE *file = fopen("access_cards.csv", "w");
    if (!file) return -1; // Could not open file for writing.

    // Write header line to CSV file
    fprintf(file, "CardNumber,Status,DateCreated\n");

    // Iterate over each accessCard and write to file
    for (size_t i = 0; i < cardCount; i++) {
        const char* statusStr = pAccessCards[i].cardAccess == ACCESS ? "ACCESS" : "NO_ACCESS";
        
        // Write the CSV line
        if (fprintf(file, "%d,%s,%ld\n", 
                    pAccessCards[i].cardNumber, 
                    statusStr, 
                    pAccessCards[i].dateCreated) < 0) {
            fclose(file);
            return -2; // Writing to file failed
        }
    }

    fclose(file); // Close the file after writing
    return 0; // Success
}

// NOTE! Caller is responsible for freeing the memory allocated by this function!
accessCard* retrieveAccessCards(size_t *cardsMallocated, size_t *cardCount) {
    FILE *file = fopen("access_cards.csv", "r");
    if (!file) return NULL;

    char line[256]; // buffer for reading a line from the .csv file. Make smaller?? 

    // Allocate space for the array in the heap
    accessCard *accessCards = malloc(*cardsMallocated * sizeof(accessCard));

    // Skip header line
    fgets(line, sizeof(line), file);

    // Check that accessCards is not NULL && check that successfully read a line from the file.
    while (accessCards && fgets(line, sizeof(line), file)) {
        // CHeck if we need to resize the array, if so DOUBLE the capacity. 
        if (*cardCount >= *cardsMallocated) {
            *cardsMallocated *= 2;
            accessCard *temp = realloc(accessCards, *cardsMallocated * sizeof(accessCard));
            if (!temp) break;
            accessCards = temp;
        }

        // Temp variables to store the parsed values
        char statusStr[20];
        unsigned int cardNumber;
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
