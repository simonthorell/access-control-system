#include "admin_menu.h"
#include <stdio.h>
#include "safeinput.h"

int adminMenu(void) {
    printMenuOptions();
    return getMenuChoice();  
}

void printMenuOptions(void){
    printf("1. Remote open door\n");
    printf("2. List all cards in system\n");
    printf("3. Add/remove access\n");
    printf("4. Exit\n");
    printf("9. FAKE TEST SCAN CARD\n");
}

int getMenuChoice(void){
    int choice;
    GetInputInt("Enter your choice: ", &choice);
    return choice;
}
