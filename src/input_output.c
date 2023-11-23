#include <stdio.h>
#include <stdarg.h>          // va_list, va_start, va_arg, va_end
#include <stdlib.h>
#include <string.h>
#include <ctype.h>          // isdigit

#include "safeinput.h"
#include "card_management.h" // validateRFIDFormat
#include "input_output.h"

/********************************************
 *              MESSAGE OUTPUT
 ********************************************/

// Call error messages only from printStatusMessage() in status_messages.c
void printSuccessMessage(const char *message) {
    printf("\033[32m* %s\033[0m\n", message);
}

void printErrorMessage(const char *message) {
    printf("\033[31m* %s\033[0m\n", message);
}

// Call info and simulation directly
void printInfoMessage(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("\033[33m* ");
    vprintf(format, args);
    printf("\033[0m\n");

    va_end(args);
}

void printSimulationMessage(const char *message) {
    printf("\033[36m* %s\033[0m\n", message);
}

/********************************************
 *                USER INPUT
 ********************************************/

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
        GetInput("\n\033[1;36mEnter password (0 = back) >>\033[0m ", passwordInput, passwordMaxLength);

        if (passwordInput[0] == '0' && passwordInput[1] == '\0') {
            return 0; // exit and return to menu
        } else if (strcmp(passwordInput, password) == 0) {
            printf("\033[32m* Password accepted!\033[0m\n");
            return 1; // return success!
        } else {
            printf("\033[31m* Wrong password! Try Again!\033[0m\n");
        }
    }
}

int getNewPassword(char* newPassword, size_t passwordMaxLength){
    while (1) {
        GetInput("\n\033[1;36mEnter NEW password (0 = back) >>\033[0m ", newPassword, passwordMaxLength);

        if (newPassword[0] == '0' && newPassword[1] == '\0') {
            return -1; // exit and return to menu
        } else if (strlen(newPassword) >= 5 && strlen(newPassword) < passwordMaxLength) {
            printf("\033[32m* New password accepted!\033[0m\n");
            return 0; // return success!
        } else {
            printf("\033[31m* Invalid password format! Try Again with 5 to %zu characters!\033[0m\n", passwordMaxLength - 1);
        }
    }
}

int getCardNumber(char* cardNumberInput, int cardIdLength){
    while (1) {
        GetInput("\n\033[1;36mEnter card RFID (0 = back) >>\033[0m ", cardNumberInput, cardIdLength);
        if (cardNumberInput[0] == '0' && cardNumberInput[1] == '\0') {
            return 0;
        } else if (isValidRFIDFormat(cardNumberInput)) {
            return 1;
        } else {
            printf("\033[31m * Invalid RFID format! Try Again!\033[0m\n");
        }
    }
}

int getConfigChoice(void* configVariable, VariableType type, char* inputMessage, int configVariableLength){
    while (1) {
        char newConfigVariable[256];  // A buffer large enough for most inputs

        printf("\n\033[1;36m%s\033[0m", inputMessage);
        GetInput(" (0 = back) \033[1;36m>>\033[0m ", newConfigVariable, sizeof(newConfigVariable) - 1);

        if (newConfigVariable[0] == '0' && newConfigVariable[1] == '\0') {
            printErrorMessage("Configuration not updated!");
            return 0;
        } else {
            if (type == TYPE_INT) {
                long val;
                char *endPtr;

                // Validate if the input is a valid integer within the specified range
                val = strtol(newConfigVariable, &endPtr, 10);  // Convert string to long
                if (endPtr == newConfigVariable || *endPtr != '\0' || val < 0 || val > configVariableLength) {
                    printErrorMessage("Invalid integer input. Please try again.");
                    continue; // Go to the next iteration of the loop
                }

                *(int*)configVariable = (int)val;  // Store the valid integer
            } else if (type == TYPE_CHAR_PTR) {
                // Check if the input is within the allowed length
                if ((int)strlen(newConfigVariable) < configVariableLength) {
                    strcpy((char*)configVariable, newConfigVariable);
                } else {
                    printErrorMessage("Input too long. Please enter a shorter input.");
                    continue; // Go to the next iteration of the loop
                }
            }
            printSuccessMessage("Configuration updated!");
            return 1;
        }
    }
}

/********************************************
 *               MENU OUTPUT
 ********************************************/

void printMenuHeader(char *menuHeader, int menuHeaderLength) {
    int stringLength = strlen(menuHeader) + 2; // +2 for the two spaces on each side of the menuHeader
    int totalPadding = menuHeaderLength - stringLength;
    int padding = totalPadding / 2;
    printf("\033[1;90m\n"); // Set color to gray
    
    for (int i = 0; i < padding; i++) {
        printf("*");
    }
    printf(" %s ", menuHeader);

    // Check if total padding is odd, if so, add one extra * to make the header symmetric
    if (totalPadding % 2 != 0) {
        padding += 1;
    }
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
    printMenuFooter(headerFooterChars);
}
