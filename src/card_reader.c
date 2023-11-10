#include <stdio.h>              // printf
#include <math.h>               // sprintf
#include <ctype.h>              // toupper
#include "safeinput.h"          // GetInputInt
#include "card_management.h"    // Struct accessCard
#include "door_control.h"       // Lock/unlock door
#include "util_sleep.h"         // portableSleep
#include "card_reader.h"

// Serial read includes
#include "connect_serial.h"     // Serial read function
#include <fcntl.h>              // open
#include <stdlib.h>             // free
#include <string.h>             // memset
#include <unistd.h>             // read

// Establish connection to door controller (ESP8266)
#include "connect_tcp_ip.h"       // sock

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

unsigned int rfidReading(accessCard *pAccessCards, size_t *pCardCount) {
    // Call serialRead() function to read from serial port. Add your own serial port path.
    int serial_port = open("/dev/cu.usbserial-110", O_RDWR); // 'ls /dev/tty.*' (list usb devices on mac)

    while (1) {
        unsigned int cardNumber = 0; // REPLACE with card ID from MCU - FAKE by typing card 1000, 1002, 1003 f.e.
        // Read from serial port
        char *line = serialRead(serial_port);
        if (line) {
            // printf("\nAccess card read with RFID: %s", line);
            cardNumber = hexToUint(line); // Convert hex string to unsigned int
            // printf("\nAccess card read with RFID: %u\n", cardNumber);
            
            // Authenticate card (will print if card is read)
            // cardNumber = 1001; // FAKE card number for testing
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

        portableSleep(500); // Wait for 0.5 second before reading again
    }
    close(serial_port); // Remember to close the port
}

int cardAuthentication(accessCard *pAccessCards, size_t *pCardCount, unsigned int cardNumber) {
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

unsigned int hexToUint(char *hexString) {
    // Convert hex string to unsigned int

    unsigned int hexValue = 0;
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

// Do not forget to free the allocated memory after use!
char *uintToHex(unsigned int cardNumber) {
    // Convert unsigned int to char*
    char *cardNumberString = malloc(sizeof(char) * 12); // 8 chars + 3 spaces + \0 = 12 chars
    if (cardNumberString == NULL) {
        printf("Error allocating memory for cardNumberString\n");
        exit(EXIT_FAILURE);
    }

    sprintf(cardNumberString, "%02X %02X %02X %02X", 
            (cardNumber >> 24) & 0xFF, 
            (cardNumber >> 16) & 0xFF, 
            (cardNumber >> 8) & 0xFF, 
            cardNumber & 0xFF);

    for (int i = 0; cardNumberString[i]; i++) {
        cardNumberString[i] = toupper(cardNumberString[i]);
    }

    return cardNumberString;
}
