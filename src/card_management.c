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
#include "input_output.h"       // print, get user input
#include "util_sleep.h"         // portableSleep
#include "status_messages.h"    // Custom error/success handling

void listAllCards(accessCard *pAccessCards, size_t *pCardCount) {
    printMenuHeader("REGISTERED ACCESS CARDS", 73); // input_output.c

    for (size_t i = 0; i < *pCardCount; i++) {
        char buffer[20]; // Buffer to hold the formatted date.

        // Format the dateCreated of the card into "YYYY-MM-DD HH:MM".
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&(pAccessCards[i].dateCreated)));

        char *cardNumberString = uintToHex(pAccessCards[i].cardNumber);

        printf("Card ID: \033[33m%s\033[0m\tStatus: %-10s\tUpdated: %s\n",
            cardNumberString,
            pAccessCards[i].cardAccess == ACCESS ? "\033[32mACCESS\033[0m  " : "\033[31mNO ACCESS\033[0m",
            buffer); // Print the formatted date.

        free(cardNumberString); // Free the allocated memory allocated by uintToHex
    }
    printMenuFooter(73); // input_output.c
}

// Add or remove access for individual RFID cards
void addRemoveAccess(accessCard *pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, unsigned long int *pCardRead) {
    unsigned long int cardNumber;

   // TODO: Move to admin_menu.c
    while (true) {
        int choice = scanCardSubMenu(); // admin_menu.c

        if (choice == 1) {
            printInfoMessage("Scan RFID card...");
            int countDown = 5;
            while (*pCardRead == 0 && countDown > 0) {
                // Wait for access card to be read on RFID reader
                portableSleep(1000); // Sleep to not consume too much CPU
                countDown--;
                if (countDown == 0) {
                    printErrorMessage("No card detected...");
                    break;
                }
            }

            if (countDown == 0) {
                continue; // Skip the rest of the loop body and start from scanCardSubMenu() again
            }

            cardNumber = *pCardRead;
            *pCardRead = 0; // Reset cardRead to 0
            break;
        } else if (choice == 2) {
            char *cardNumberInput = malloc(sizeof(char) * CARD_ID_LENGTH); // 8 chars + 3 spaces + \0 = 12 chars => CHANGE TO GLOBAL VARIABLE?
            int cardEntered = getCardNumber(cardNumberInput, CARD_ID_LENGTH); // admin_menu.c

            if (cardEntered == 0) {
                free(cardNumberInput);
                continue;
            }

            cardNumber = hexToUint(cardNumberInput);
            free(cardNumberInput);
            break;
        } else if (choice == 3) {
            return;
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
            printStatusMessage(SUCCESS, "Card with RFID '%s' found with current access status: %s", uintToHex(cardNumber), pAccessCards[middle].cardAccess == ACCESS ? "\033[32mACCESS\033[0m  " : "\033[31mNO ACCESS\033[0m");
            updateCardSubMenu(pAccessCards, pCardCount, middle);
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
            printStatusMessage(ERROR_GENERAL, "Card with RFID '%s' not found", cardNumberString);
            int choice = addNewCardSubMenu(); // admin_menu.c
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

void addNewCard(accessCard **pAccessCards, size_t *pCardsMallocated, size_t *pCardCount, int cardIndex, unsigned long int cardNumber) {
    // Check if there is space for card within current memory allocation, if not - double allocation. 
    if (*pCardCount >= *pCardsMallocated) {
        size_t oldSize = *pCardsMallocated * sizeof(accessCard);
        printInfoMessage("Previous allocated memory size: %zu bytes", oldSize);
        // Attempt to double the amount of allocated memory
        *pCardsMallocated *= 2;
        
        accessCard *temp = realloc(*pAccessCards, *pCardsMallocated * sizeof(accessCard*));

        size_t newSize = *pCardsMallocated * sizeof(accessCard);
        printInfoMessage("New allocated memory size: %zu bytes", newSize);

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
    printInfoMessage("%zu cards stored and current allocation is %zu cards", *pCardCount, *pCardsMallocated);

    // Create new card at new empty spot (cardIndex) in array
    (*pAccessCards)[cardIndex].cardNumber = cardNumber;
    setCardAccess(*pAccessCards, cardIndex);

    // TODO: change the uintToHex function to not use malloc?
    char *cardNumberString = uintToHex(cardNumber);
    printStatusMessage(SUCCESS, "New card with ID '%s' has been registered", cardNumberString);
    free(cardNumberString); // Free the allocated memory
}

void setCardAccess(accessCard *pAccessCards, size_t cardIndex) {
    int cardAccess;
    cardAccess = updateAccessMenu(); // admin_menu.c
    pAccessCards[cardIndex].cardAccess = cardAccess;
    pAccessCards[cardIndex].dateCreated = time(NULL); // Generate current date/time
    printStatusMessage(SUCCESS, "Card updated with access status: %s", pAccessCards[cardIndex].cardAccess == ACCESS ? "\033[32mACCESS\033[0m" : "\033[31mNO ACCESS\033[0m");
}


void removeCard(accessCard **pAccessCards, size_t *pCardCount, size_t cardIndex) {
    printInfoMessage("Removing card with ID '%s'...", uintToHex((*pAccessCards)[cardIndex].cardNumber));

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
