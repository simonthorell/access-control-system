#include <stdio.h>              // printf
#include <math.h>               // sprintf
#include <ctype.h>              // toupper
#include <stdlib.h>             // free
#include <string.h>             // memset
#include <unistd.h>             // read
#include "safeinput.h"          // GetInputInt
#include "card_management.h"    // Struct accessCard
#include "door_control.h"       // Lock/unlock door
#include "card_reader.h"        // cardAuthentication
#include "connect_serial.h"     // Serial read function
#include "input_output.h"       // getRFIDCardNumber

// Establish connection to door controller (ESP8266)
#include "connect_tcp_ip.h"       // sock

void fakeTestScanCard(accessCard *pAccessCards, size_t *pCardCount) {
    // Simulate the scanning of an RFID card for testing purposes
    char *cardNumberInput = malloc(sizeof(char) * CARD_ID_LENGTH); // 8 chars + 3 spaces + \0 = 12 chars => CHANGE TO GLOBAL VARIABLE?
    int returnValue = getCardNumber(cardNumberInput, CARD_ID_LENGTH); // admin_menu.c
    if (returnValue == 0) {
        return; // Return to admin menu
    }

    unsigned long int cardNumber = hexToUint(cardNumberInput); // Convert hex string to unsigned int
    int cardAuthenticated = cardAuthentication(pAccessCards, pCardCount, cardNumber);

    if (cardAuthenticated) {
        printf("Card authenticated! Access granted!\n");
        lockUnlockMechanism(DOOR_UNLOCKED);
        // Validate that door controller MCU received and successfully executed locking mechanism. 
        if (!doorStatus) {
            // printf("Door successfully unlocked!\n");
        }
    } else {
        printf("Card authenticated! Access denied!\n");
        lockUnlockMechanism(DOOR_LOCKED);
        // Validate that door controller MCU received and successfully executed locking mechanism. 
        if (doorStatus) {
            // printf("Door currently locked!\n");
        }
    }

    free(cardNumberInput);
}

unsigned long int rfidReading(volatile bool *runCardReaderThread, accessCard *pAccessCards, size_t *pCardCount, int serial_port) {
    while (*runCardReaderThread) {
        unsigned long int cardNumber = 0; // REPLACE with card ID from MCU - FAKE by typing card 1000, 1002, 1003 f.e.
        // Read from serial port
        char *line = serialRead(serial_port, 1); // This function waits for serialRead and then continues
        if (line) {
            cardNumber = hexToUint(line); // Convert hex string to unsigned int
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
            free(line); // Free the allocated memory
            return cardNumber; // Return card number
        }
    }
    return 0;
}

int cardAuthentication(accessCard *pAccessCards, size_t *pCardCount, unsigned long int cardNumber) {
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

unsigned long int hexToUint(char *hexString) {
    // Convert hex string to unsigned int

    unsigned long int hexValue = 0;
    for (size_t i = 0; i < strlen(hexString); i++) {
        if (hexString[i] == ' ') {
            continue;
        }

        if (hexString[i] >= '0' && hexString[i] <= '9') {
            hexValue = hexValue * 16 + (hexString[i] - '0');
        } else if (hexString[i] >= 'a' && hexString[i] <= 'f') {
            hexValue = hexValue * 16 + (hexString[i] - 'a' + 10);
        } else if (hexString[i] >= 'A' && hexString[i] <= 'F') {
            hexValue = hexValue * 16 + (hexString[i] - 'A' + 10);
        }
    }
    
    return hexValue;
}

// Convert unsigned int to char* - Do not forget to free the allocated memory after use!
char *uintToHex(unsigned long int cardNumber) {
    char *cardNumberString = malloc(sizeof(char) * 12); // 8 chars + 3 spaces + \0 = 12 chars // Only works for 32-bit unsigned long int (4 bytes)
    // char *cardNumberString = malloc(sizeof(char) * 24); // Modified for 64-bit unsigned long int (8 bytes)
    if (cardNumberString == NULL) {
        printf("Error allocating memory for cardNumberString\n");
        exit(EXIT_FAILURE);
    }

    sprintf(cardNumberString, "%02lX %02lX %02lX %02lX", 
            (cardNumber >> 24) & 0xFF, 
            (cardNumber >> 16) & 0xFF, 
            (cardNumber >> 8) & 0xFF, 
            cardNumber & 0xFF);

    for (int i = 0; cardNumberString[i]; i++) {
        cardNumberString[i] = toupper(cardNumberString[i]);
    }

    return cardNumberString;
}
