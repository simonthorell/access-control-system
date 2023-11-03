#include "door_control.h"
#include <stdio.h>

/* OS portable sleep function:
   If _WIN32 is defined, we are on a Windows system else Unix-based system.
   Static tells the compiler that the function has internal linkage, which means it won't be visible to other translation units. 
*/
#if defined(_WIN32)
    #include <windows.h>
    static void portableSleep(unsigned int seconds) {
        Sleep(seconds * 1000); // The argument to Sleep is in milliseconds
    }
#else
    #include <unistd.h>
    static void portableSleep(unsigned int seconds) {
        sleep(seconds); // The argument to sleep is in seconds
    }
#endif

// Open door from admin menu
void remoteOpenDoor(void) {
    printf("Door unlocked remotely.\n");
    lockUnlockMechanism(DOOR_UNLOCKED);
}

// Sends signal to MCU to unlock the door
void lockUnlockMechanism(int doorLock) {
    switch (doorLock) {
        case DOOR_UNLOCKED:
            // Code to send signal to MCU to unlock door => GREEN LED
            printf("CURRENTLY LAMP IS: Green\n");
            // KEEP DOOR UNLOCKED FOR 3 SECONDS
            portableSleep(3);
            lockUnlockMechanism(DOOR_LOCKED);
            break;
        case DOOR_LOCKED:
            // Code to send signal to MCU to lock door => RED LED
            printf("CURRENTLY LAMP IS: Red\n");
            break;
    }
}
