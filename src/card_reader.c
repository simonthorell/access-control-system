//#include "card_reader.h"
#include <stdio.h>
#include "safeinput.h"
#include "card_management.h"
#include "card_reader.h"
#include "door_control.h"

void fakeTestScanCard(accessCard *pAccessCards, size_t *pCardCount) {
    // Simulate the scanning of an RFID card for testing purposes
    int cardNumber;
    GetInputInt("Enter fake card ID: ", &cardNumber);
    // printf("Fake RFID card scanned...\n");

    int cardAuthenticated = cardAuthentication(pAccessCards, pCardCount, cardNumber);

    if (cardAuthenticated) {
        printf("Card authenticated!\n");
        lockUnlockMechanism(DOOR_UNLOCKED);
    }
    else {
        printf("Card not authenticated!\n");
    }

}

void rfidReading(void) {
    // Code to handle the reading of RFID card data from MCU
    printf("Reading RFID card...\n");
}

int cardAuthentication(accessCard *pAccessCards, size_t *pCardCount, int cardNumber) {
    // Code to validate RFID card against the authorized list
    // printf("Authenticating RFID card...\n");

    // Loop through all registered cards
    for (size_t i = 0; i < *pCardCount; i++) {
        if (pAccessCards[i].cardNumber == cardNumber) {
            // Check if card has access
            if (pAccessCards[i].cardAccess == ACCESS) {
                return ACCESS;
            }
            else {
                return NO_ACCESS;
            }
            break;
        }
    }

    return NO_ACCESS; // FALL BACK = NO ACCESS
}
