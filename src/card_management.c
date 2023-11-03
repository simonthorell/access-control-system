#include "card_management.h"
#include <stdio.h>
#include "safeinput.h"
#include "data_storage.h"

void listAllCards(void) {
    printf("Listing all registered access cards...\n");
    
    size_t cardCount;
    accessCard *pAccessCards = retrieveAccessCards(&cardCount);

    // Iterate over the cards using cardCount to avoid hardcoding the size
    for (size_t i = 0; i < cardCount; i++) {
        char buffer[20]; // Buffer to hold the formatted date.

        // Format the dateCreated of the card into "YYYY-MM-DD HH:MM".
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&(pAccessCards[i].dateCreated)));

        printf("Card ID: %-10d\tStatus: %-10s\tUpdated: %s\n",
            pAccessCards[i].cardNumber,
            pAccessCards[i].cardAccess == ACCESS ? "ACCESS" : "NO ACCESS",
            buffer); // Use buffer here in the printf statement.
    }
}

// Add or remove access for individual RFID cards
void addRemoveAccess(void) {
    int cardNumber;
    int cardAccess;
    GetInputInt("Enter cardnumber: ", &cardNumber);

    // Check if card number is in array of registered cards, assign access value to cardAccess else cardAccess = NO_ACCESS.
    // Check if card number is in array of registered cards, else last card ID + 1

    // if (cardNumber-->access == ACCESS) {
    //     printf("Card currently %d has access\n", cardnumber);
    // } else if (cardNumber-->access == NO_ACCESS) {
    //     printf("Card currently%d has no access\n", cardnumber);
    // } else {
    //     time_t dateNow = time(NULL); // Generate current date/time
    //     printf("New card with ID %d has been registered\n", cardnumber);
    // }


    GetInputInt("Enter new card access (0 = no access, 1 = access): ", &cardAccess);
    
    if (cardAccess == NO_ACCESS) {
        printf("Removing access for card %d\n", cardNumber);
    } else if (cardAccess == ACCESS) {
        printf("Adding access for card %d\n", cardNumber);
    } else {
        printf("Invalid access value: %d\n", cardAccess);
    }
}
