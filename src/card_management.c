#include "card_management.h"
#include <stdio.h>
#include "safeinput.h"

void listAllCards(void) {
    // List all RFID cards from the system's database
    printf("Listing all registered RFID cards...\n");
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
