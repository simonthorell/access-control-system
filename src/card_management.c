#include "card_management.h"
#include <stdio.h>
#include "safeinput.h"
#include "data_storage.h"
#include "card_reader.h"

void listAllCards(accessCard *pAccessCards, size_t *pCardCount) {
    printf("Listing all registered access cards...\n");

    for (size_t i = 0; i < *pCardCount; i++) {
        char buffer[20]; // Buffer to hold the formatted date.

        // Format the dateCreated of the card into "YYYY-MM-DD HH:MM".
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&(pAccessCards[i].dateCreated)));

        printf("Card ID: %-10d\tStatus: %-10s\tUpdated: %s\n",
            pAccessCards[i].cardNumber,
            pAccessCards[i].cardAccess == ACCESS ? "ACCESS" : "NO ACCESS",
            buffer); // Print the formatted date.
    }
}

// Add or remove access for individual RFID cards
void addRemoveAccess(accessCard *pAccessCards, size_t *pCardCount) {
    int cardNumber;
    int cardAccess;
    bool cardFound = false;

    GetInputInt("Enter card ID: ", &cardNumber);

    // Check if card exists in access card array
    for (size_t i = 0; i < *pCardCount; i++) {
        if (pAccessCards[i].cardNumber == cardNumber) {
            printf("Card currently has access: %d (0 = NO ACCESS, 1 = ACCESS)\n", pAccessCards[i].cardAccess);
            GetInputInt("Enter new card access (0 = NO ACCESS, 1 = ACCESS): ", &cardAccess);
            printf("Updated card with access: %d\n", cardAccess);
            pAccessCards[i].cardAccess = cardAccess;
            pAccessCards[i].dateCreated = time(NULL); // Generate current date/time
            cardFound = true;
            break;
        }
    }

    // If card was not found, register it
    if (!cardFound) {
        // TO DO: REALLOC!!!
        printf("New card with ID '%d' has been registered\n", cardNumber);
        pAccessCards[(*pCardCount)].cardNumber = cardNumber;    
        GetInputInt("Enter new card access (0 = NO ACCESS, 1 = ACCESS): ", &cardAccess);
        printf("Updated card with access: %d\n", cardAccess);
        pAccessCards[(*pCardCount)].cardAccess = cardAccess;
        pAccessCards[(*pCardCount)].dateCreated = time(NULL); // Generate current date/time
        (*pCardCount)++;
    }
    
}
