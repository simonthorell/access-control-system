#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>              // isxdigit
#include <stdbool.h>            // bool
#include <string.h>             // strlen

#include "admin_menu.h"         // I/O functions
#include "card_management.h"    // time.h
#include "safeinput.h"
#include "data_storage.h"
#include "card_reader.h"

void listAllCards(accessCard *pAccessCards, size_t *pCardCount) {
    printf("\n\033[1;90m************************ REGISTERED ACCESS CARDS ************************\033[0m\n");

    for (size_t i = 0; i < *pCardCount; i++) {
        char buffer[20]; // Buffer to hold the formatted date.

        // Format the dateCreated of the card into "YYYY-MM-DD HH:MM".
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&(pAccessCards[i].dateCreated)));

        char *cardNumberString = malloc(sizeof(char) * CARD_ID_LENGTH); // 8 chars + 3 spaces + \0 = 12 chars
        cardNumberString = uintToHex(pAccessCards[i].cardNumber); // card_reader.c

        printf("Card ID: %s\tStatus: %-10s\tUpdated: %s\n",
            cardNumberString,
            pAccessCards[i].cardAccess == ACCESS ? "ACCESS" : "NO ACCESS",
            buffer); // Print the formatted date.

        free(cardNumberString);
    }
    printf("\033[1;90m*************************************************************************\033[0m\n");
}

// Add or remove access for individual RFID cards
void addRemoveAccess(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned int *pCardRead) {
    unsigned int cardNumber;
    int choice = ScanCardSubMenu(); // admin_menu.c
   
    while (true) {
        if (choice == 1) {
            printf("Scan RFID card...\n");
            while (*pCardRead == 0) {
                // TODO: Add timeout - Wait for card to be read by MCU RFID card reader
            }
            cardNumber = *pCardRead;
            break;
        } else if (choice == 2) {
            char *cardNumberInput = malloc(sizeof(char) * CARD_ID_LENGTH); // 8 chars + 3 spaces + \0 = 12 chars => CHANGE TO GLOBAL VARIABLE?
            getCardNumber(cardNumberInput, CARD_ID_LENGTH); // admin_menu.c
            cardNumber = hexToUint(cardNumberInput);
            free(cardNumberInput);
            break;
        } else if (choice == 3) {
            printAdminMenu();
            return;
        } else {
            printf("Invalid choice! Try Again! \n");
        }
    }

    // Binary search for card number
    int left = 0;
    int right = *pCardCount - 1;
    int middle;
    bool found = false;

    while (left <= right) {
        middle = left + (right - left) / 2; // To prevent potential overflow
        if (pAccessCards[middle].cardNumber == cardNumber) {
            const char *accessStrings[] = {"NO ACCESS", "ACCESS"};
            printf("Card with RFID '%s' found with current access status: %s\n", uintToHex(*pCardRead), accessStrings[pAccessCards[middle].cardAccess]);
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
        // Ask to add new card
        while (true) {
            char *cardNumberString = uintToHex(cardNumber);
            int choice = addNewCardSubMenu(cardNumberString); // admin_menu.c
            free(cardNumberString); // Free the allocated memory

            if (choice == 1) {
                // If the card is not found, the `left` variable now points to the position where the card should be inserted.
                addNewCard(&pAccessCards, pCardsMallocated, pCardCount, left, cardNumber);
                break;
            } else {
                break;
            }
        }
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
    printf("New card with ID '%s' has been registered.\n", uintToHex(cardNumber));
}

void updateCard(accessCard *pAccessCards, size_t *pCardCount, size_t cardIndex) {
    int choice = updateCardSubMenu(); // admin_menu.c

    switch (choice) {
        case 1:
            setCardAccess(pAccessCards, cardIndex);
            break;
        case 2:
            removeCard(&pAccessCards, pCardCount, cardIndex);
            break;
        case 3:
            printAdminMenu();
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
    printf("Removing card with ID '%s'...\n", uintToHex((*pAccessCards)[cardIndex].cardNumber));

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

bool isValidRFIDFormat(char *rfid) {
    int len = strlen(rfid);

    // Check total length for both formats
    if (len != 11 && len != 8) {
        return false;
    }

    // Check and format the 'XXXXXXXX' format
    if (len == 8) {
        for (int i = 0; i < len; i++) {
            if (!isxdigit(rfid[i])) {
                return false;
            }
        }
        // Reformat to 'XX XX XX XX' by moving characters and inserting spaces
        for (int i = len - 1; i >= 0; i--) {
            int targetIndex = i + (i / 2); // Calculate the target index considering the spaces
            rfid[targetIndex] = rfid[i];
            if (i % 2 == 0 && i != 0) {
                rfid[targetIndex - 1] = ' '; // Insert space
            }
        }
        rfid[11] = '\0'; // Null-terminate the string
        return true;
    }

    // Check the 'XX XX XX XX' format
    if (len == 11) {
        for (int i = 0; i < len; i++) {
            if ((i == 2 || i == 5 || i == 8) && rfid[i] != ' ') {
                return false;
            } else if (i != 2 && i != 5 && i != 8 && !isxdigit(rfid[i])) {
                return false;
            }
        }
        return true;
    }

    return false;
}
