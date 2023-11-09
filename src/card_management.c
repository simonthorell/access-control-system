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

        char *cardNumberString = malloc(sizeof(char) * 12); // 8 chars + 3 spaces + \0 = 12 chars
        cardNumberString = uintToHex(pAccessCards[i].cardNumber); // card_reader.c
        // char *cardNumberString = uintToHex(pAccessCards[i].cardNumber); // card_reader.c

        printf("Card ID: %s\tStatus: %-10s\tUpdated: %s\n",
            cardNumberString,
            pAccessCards[i].cardAccess == ACCESS ? "ACCESS" : "NO ACCESS",
            buffer); // Print the formatted date.

        free(cardNumberString);

        // For testing! Print the unsigned int card number (cards are sorted by unsigned int card number).
        // printf("Card ID: %-10u\tStatus: %-10s\tUpdated: %s\n",
        //     pAccessCards[i].cardNumber,
        //     pAccessCards[i].cardAccess == ACCESS ? "ACCESS" : "NO ACCESS",
        //     buffer); // Print the formatted date.
    }
}

// Add or remove access for individual RFID cards
void addRemoveAccess(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount) {
    char *cardNumberInput = malloc(sizeof(char) * 12); // 8 chars + 3 spaces + \0 = 12 chars => CHANGE TO GLOBAL VARIABLE? 
    GetInput("Enter card ID: ", cardNumberInput, 12);
    unsigned int cardNumber = hexToUint(cardNumberInput);
    // printf("Card ID: %s\n", cardNumberInput);
    // printf("Card ID: %u\n", cardNumber);
    free(cardNumberInput);

    // Binary search for card number
    int left = 0;
    int right = *pCardCount - 1;
    int middle;
    bool found = false;

    while (left <= right) {
        middle = left + (right - left) / 2; // To prevent potential overflow
        if (pAccessCards[middle].cardNumber == cardNumber) {
            const char *accessStrings[] = {"NO ACCESS", "ACCESS"};
            printf("Card with ID '%d' found with current access status: %s\n", cardNumber, accessStrings[pAccessCards[middle].cardAccess]);
            updateCard(pAccessCards, pCardCount, middle);
            found = true;
            break;
        } else if (cardNumber < pAccessCards[middle].cardNumber) {
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }

    // If the card is not found, the `left` variable now points to the position where the card should be inserted.
    if (!found) {
        // If the card is not found, the `left` variable now points to the position where the card should be inserted.
        addNewCard(&pAccessCards, pCardsMallocated, pCardCount, left, cardNumber);
    }
    
}

void addNewCard(accessCard **pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, int cardIndex, unsigned int cardNumber) {
    // Check if there is space for card within current memory allocation, if not - double allocation. 
    if (*pCardCount >= *pCardsMallocated) {
        // Attempt to double the amount of allocated memory
        *pCardsMallocated *= 2;
        accessCard *temp = realloc(*pAccessCards, *pCardsMallocated * sizeof(accessCard*));
        if (!temp) {
            // If realloc fails, we print an error message and return early to avoid writing into a null pointer.
            fprintf(stderr, "Error: realloc failed.\n");
            return;
        }
        // If realloc succeeds, we update the original pointer to point to the newly allocated memory.
        *pAccessCards = temp;
    }

    // Loop through array and shift all elements after cardIndex to the right by one position.
        for (size_t i = *pCardCount; i > (size_t)cardIndex; i--) {
        (*pAccessCards)[i] = (*pAccessCards)[i - 1];
    }
    (*pCardCount)++;

    printf("%zu cards stored and current allocation is %zu cards.\n", *pCardCount, *pCardsMallocated);

    // Create new card at new empty spot (cardIndex) in array
    
    (*pAccessCards)[cardIndex].cardNumber = cardNumber;
    setCardAccess(*pAccessCards, cardIndex);
    printf("New card with ID '%d' has been registered.\n", cardNumber);
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
