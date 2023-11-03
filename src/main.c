#include <stdio.h>
#include <stdbool.h>
#include "admin_menu.h"

int main(void) {
    bool runProgram = true;

    do {
        int menuChoice = adminMenu();

        switch (menuChoice) {
            case REMOTE_OPEN_DOOR:
                // remoteOpenDoor();
                break;
            case LIST_CARDS:
                // listCards();
                break;
            case ADD_REMOVE_ACCESS:
                // addRemoveAccess();
                break;
            case EXIT:
                // free memory
                runProgram = false;
                break;
            case FAKE_TEST_SCAN_CARD:
                // fakeTestScanCard();
                break;
            default:
                printf("Invalid choice! Try Again! \n");
                break;
        }
    } while (runProgram);
    
    return 0;
}
