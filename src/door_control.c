#include "door_control.h"
#include <stdio.h>

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
