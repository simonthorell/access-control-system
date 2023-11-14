#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include "admin_menu.h" // MenuOption struct

// User input
int getMenuChoice(MenuOption *options, size_t menuOptionsSize);
int getCardNumber(char* cardNumberInput, int cardIdLength);

// Print menu
void printMenuHeader(char *menuHeader, int menuHeaderLength);
void printMenuOptions(MenuOption *options, size_t menuOptionsSize);
void printMenuFooter(int menuHeaderLength);
void printMenu(char *menuHeader, int headerFooterChars, MenuOption *options, size_t menuOptionsSize);

#endif // INPUT_OUTPUT_H
