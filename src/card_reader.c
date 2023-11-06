#include <stdio.h>              // printf
#include "safeinput.h"          // GetInputInt
#include "card_management.h"    // Struct accessCard
#include "door_control.h"       // Lock/unlock door
#include "card_reader.h"

void fakeTestScanCard(accessCard *pAccessCards, size_t *pCardCount) {
    // Simulate the scanning of an RFID card for testing purposes
    int cardNumber;
    GetInputInt("Enter fake card ID: ", &cardNumber);
    int cardAuthenticated = cardAuthentication(pAccessCards, pCardCount, cardNumber);

    if (cardAuthenticated) {
        printf("Card authenticated!\n");
        lockUnlockMechanism(DOOR_UNLOCKED);
    } else {
        printf("Card not authenticated!\n");
        lockUnlockMechanism(DOOR_LOCKED);
    }
}

void rfidReading(accessCard *pAccessCards, size_t *pCardCount) {
    // TODO: scanner for RFID card data from MCU - Below for testing purposes
    int cardNumber = 0; // REPLACE with card ID from MCU - FAKE by typing card 1000, 1002, 1003 f.e.
    
    if (cardNumber != 0) {
        // Authenticate card (will print if card is read)
        int cardAuthenticated = cardAuthentication(pAccessCards, pCardCount, cardNumber);
        // If card is authenticated, unlock door
        if (cardAuthenticated) {
            lockUnlockMechanism(DOOR_UNLOCKED);
        } else {
            lockUnlockMechanism(DOOR_LOCKED);
        }
    }
    cardNumber = 0; // Reset card number
}

int cardAuthentication(accessCard *pAccessCards, size_t *pCardCount, int cardNumber) {
    // Loop through all registered cards to validate RFID card against the authorized list
    for (size_t i = 0; i < *pCardCount; i++) {
        if (pAccessCards[i].cardNumber == cardNumber) {
            // Check if card has access
            if (pAccessCards[i].cardAccess == ACCESS) {
                return ACCESS;
            } else {
                return NO_ACCESS;
            }
            break;
        }
    }
    return NO_ACCESS; // FALL BACK = NO ACCESS (0)
}
