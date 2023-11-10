#include "door_control.h"
#include <stdio.h>
#include "util_sleep.h"
#include "connect_tcp_ip.h" // Connect to Wemos D1 Mini controlling door lock

// Define a variable to track the current door status
int doorStatus = DOOR_LOCKED;

// Open door from admin menu
void remoteOpenDoor(void) {
    printf("Remote unlocking door...\n");
    lockUnlockMechanism(DOOR_UNLOCKED);
    (printf("Door unlocked!\n"));
}

// Sends signal to MCU to unlock the door
void lockUnlockMechanism(int doorLock) {
    int result;

    switch (doorLock) {
        case DOOR_UNLOCKED:
            // Code to send signal to MCU to unlock door => GREEN LED
            result = wifiWrite("DOOR_UNLOCKED"); // connect_wifi.c
            if (result == 0) {
                doorStatus = DOOR_UNLOCKED;
            }
            // Code for testing only
            // printf("CURRENTLY LAMP IS: Green\n");
            portableSleep(100); // Wait to time LED on door controller (otherwise not queing up correct on MCU).
            lockUnlockMechanism(DOOR_LOCKED);
            break;
        case DOOR_LOCKED:
            // Code to send signal to MCU to lock door => RED LED
            result = wifiWrite("DOOR_LOCKED"); // connect_wifi.c
            if (result == 0) {
                doorStatus = DOOR_LOCKED;
            }
            // Code for testing only
            // printf("CURRENTLY LAMP IS: Red\n");
            // portableSleep(3);
            // printf("CURRENTLY LAMP IS: Off\n");
            break;
    }
}
