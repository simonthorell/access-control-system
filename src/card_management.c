#include <stdio.h>
#include <stdlib.h>

#include "card_management.h"
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
    bool cardFound = false;

    GetInputInt("Enter card ID: ", &cardNumber);

    // Check if card exists in access card array
    for (size_t i = 0; i < *pCardCount; i++) {
        if (pAccessCards[i].cardNumber == cardNumber) {
            cardFound = true;
            const char *accessStrings[] = {"NO ACCESS", "ACCESS"};
            printf("Card with ID '%d' found with current access status: %s\n", cardNumber, accessStrings[pAccessCards[i].cardAccess]);
            updateCard(pAccessCards, pCardCount, i);
            break;
        }
    }

    // If card was not found, register it
    if (!cardFound) {
        // TO DO: REALLOC!!!
        printf("New card with ID '%d' has been registered.\n", cardNumber);
        pAccessCards[(*pCardCount)].cardNumber = cardNumber;
        setCardAccess(pAccessCards, *pCardCount);
        (*pCardCount)++;
    }
    
}

void updateCard(accessCard *pAccessCards, size_t *pCardCount, size_t cardIndex) {
    printf("1. Change access\n");
    printf("2. Remove card\n");
    printf("3. Exit\n");

    int choice;
    GetInputInt("Enter your choice (enter 0 to show options): ", &choice);

    switch (choice) {
        case 1:
            setCardAccess(pAccessCards, cardIndex);
            break;
        case 2:
            removeCard(&pAccessCards, pCardCount, cardIndex);
            break;
        case 3:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice! Try Again! \n");
            break;
    }
}

void setCardAccess(accessCard *pAccessCards, size_t cardIndex) {
    int cardAccess;
    const char *accessStrings[] = {"NO ACCESS", "ACCESS"};

    GetInputInt("Enter new card access (0 = NO ACCESS, 1 = ACCESS): ", &cardAccess);
    printf("Card updated with access status: %s\n", accessStrings[pAccessCards[cardIndex].cardAccess]);
    pAccessCards[cardIndex].cardAccess = cardAccess;
    pAccessCards[cardIndex].dateCreated = time(NULL); // Generate current date/time
}


void removeCard(accessCard **pAccessCards, size_t *pCardCount, size_t cardIndex) {
    // printf("Card count: %zu\n", *pCardCount);
    // printf("Card index '%zu'...\n", cardIndex);
    printf("Removing card with ID '%d'...\n", (*pAccessCards)[cardIndex].cardNumber);

    // Check if the cardIndex is valid
    if (cardIndex >= *pCardCount) {
        fprintf(stderr, "Error: cardIndex is out of bounds.\n");
        return;
    }

    // Shift all elements after cardIndex to the left by one position
    for (size_t i = cardIndex; i < *pCardCount - 1; i++) {
        (*pAccessCards)[i] = (*pAccessCards)[i + 1];
    }

    // Reduce the card count by one
    (*pCardCount)--;

    // Reallocate the memory to shrink the array size
    accessCard *temp = realloc(*pAccessCards, (*pCardCount) * sizeof(accessCard));
    if (temp == NULL && *pCardCount > 0) {
        fprintf(stderr, "Error: realloc failed.\n");
        return;
    }
    *pAccessCards = temp;
}
