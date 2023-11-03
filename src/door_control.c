#include "door_control.h"
#include <stdio.h>
#include <stdbool.h>

void remoteOpenDoor(void) {
    // Sends signal to Arduino to unlock the door
    lockUnlockMechanism(DOOR_UNLOCKED);
    printf("Door unlocked remotely.\n");
}

void lockUnlockMechanism(int lockState) {
    if (lockState) {
        // Code to show GREEN LED on Arduino
        // printf("Door locked.\n");
    } else {
        // Code to show RED LED on Arduino
        // rintf("Door unlocked.\n");
    }
}
