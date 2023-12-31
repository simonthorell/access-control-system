#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include "admin_menu.h" // MenuOption struct

// Used in getConfigChoice() to determine which type of variable we are dealing with
typedef enum {
    TYPE_INT,
    TYPE_CHAR_PTR
} VariableType;

// User input
int getMenuChoice(MenuOption *options, size_t menuOptionsSize);
int getPassword(char* password, int passwordMaxLength);
int getNewPassword(char* newPassword, size_t passwordMaxLength);
int getCardNumber(char* cardNumberInput, int cardIdLength);
int getConfigChoice(void* configVariable, VariableType type, char* inputMessage, int configVariableLength);

// Print menu
void printMenuHeader(char *menuHeader, int menuHeaderLength);
void printMenuOptions(MenuOption *options, size_t menuOptionsSize);
void printMenuFooter(int menuHeaderLength);
void printMenu(char *menuHeader, int headerFooterChars, MenuOption *options, size_t menuOptionsSize);

// Print messages
void printSuccessMessage(const char *message);
void printErrorMessage(const char *message);
void printInfoMessage(const char *format, ...);
void printSimulationMessage(const char *message);

#endif // INPUT_OUTPUT_H
