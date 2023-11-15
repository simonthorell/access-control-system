#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "safeinput.h"
#include "card_management.h" // validateRFIDFormat
#include "input_output.h"

// User input
int getMenuChoice(MenuOption *options, size_t menuOptionsSize){
    while (1) {
        int choice;
        // Validate that GetInputInt returns true and that the choice is in the menuOptions array
        if (GetInputInt("\n\033[1;36mEnter command >>\033[0m ", &choice)) {
            if (choice == 0) {
                return choice; // Return 0 to show menu options again
                break;
            }
            for (size_t i = 0; i < menuOptionsSize; i++) {
                if (choice == options[i].key) {
                    return choice;
                    break;
                }
            }
        }
        printf("Invalid choice! Try Again! \n");
    }
    return 1; // Function did not return a valid value
}

int getPassword(char* password, int passwordMaxLength){
    while (1) {
        char passwordInput[passwordMaxLength];
        GetInput("\n\033[1;36mEnter password (0 = exit) >>\033[0m ", passwordInput, passwordMaxLength);

        if (passwordInput[0] == '0') {
            return 0; // exit and return to menu
        } else if (strcmp(passwordInput, password) == 0) {
            printf("\033[32m* Password accepted!\033[0m\n");
            return 1; // return success!
        } else {
            printf("\033[31m* Invalid password format! Try Again!\033[0m\n");
        }
    }
}

int getCardNumber(char* cardNumberInput, int cardIdLength){
    while (1) {
        GetInput("\n\033[1;36mEnter card RFID (0 = exit) >>\033[0m ", cardNumberInput, cardIdLength);
        if (cardNumberInput[0] == '0') {
            return 0;
        } else if (isValidRFIDFormat(cardNumberInput)) {
            return 1;
        } else {
            printf("\033[31m * Invalid RFID format! Try Again!\033[0m\n");
        }
    }
}

void printMenuHeader(char *menuHeader, int menuHeaderLength) {
    int stringLength = strlen(menuHeader) + 2; // +2 for the two spaces on each side of the menuHeader
    int padding = (menuHeaderLength - stringLength) / 2;
    printf("\033[1;90m\n"); // Set color to gray
    
    for (int i = 0; i < padding; i++) {
        printf("*");
    }
    printf(" %s ", menuHeader);

    for (int i = 0; i < padding; i++) {
        printf("*");
    }
    printf("\033[0m\n"); // Reset color
}

void printMenuOptions(MenuOption *options, size_t menuOptionsSize) {
    for (size_t i = 0; i < menuOptionsSize; i++) {
        printf("%d. %s\n", options[i].key, options[i].value);
    }
}

void printMenuFooter(int menuHeaderLength) {
    for (int i = 0; i < menuHeaderLength; i++) {
        printf("\033[1;90m*"); // Set color to gray
    }
    printf("\033[0m\n"); // Reset color
}

void printMenu(char *menuHeader, int headerFooterChars, MenuOption *options, size_t menuOptionsSize) {
    printMenuHeader(menuHeader, headerFooterChars);
    printMenuOptions(options, menuOptionsSize);
    // printMenuFooter(headerFooterChars);

    // If the header char amount is uneven, remove one char from the footer to match length. 
    if (headerFooterChars % 2 == 0) {
        printMenuFooter(headerFooterChars - 1);
    } else {
        printMenuFooter(headerFooterChars);
    }
}
