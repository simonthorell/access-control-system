#include "door_control.h"
#include <stdio.h>
// #include "util_sleep.h"
#include "connect_wifi.h" // Connect to Wemos D1 Mini controlling door lock

// Open door from admin menu
void remoteOpenDoor(void) {
    printf("Door unlocked remotely.\n");
    lockUnlockMechanism(DOOR_UNLOCKED);
}

// Sends signal to MCU to unlock the door
void lockUnlockMechanism(int doorLock) {
    // Establish connection to door lock system
    if (establishConnection("192.168.50.254") != 0) {
        // Handle error
        printf("Error establishing connection\n");
        // return -1;
    }

        switch (doorLock) {
            case DOOR_UNLOCKED:
                // Code to send signal to MCU to unlock door => GREEN LED
                wifiWrite("DOOR_UNLOCKED"); // connect_wifi.c

                // Code for testing only
                // printf("CURRENTLY LAMP IS: Green\n");
                // portableSleep(3); // KEEP DOOR UNLOCKED FOR 3 SECONDS
                // wifiWrite("DOOR_LOCKED"); // connect_wifi.c
                // lockUnlockMechanism(DOOR_UNLOCKED);
                break;
            case DOOR_LOCKED:
                // Code to send signal to MCU to lock door => RED LED
                wifiWrite("DOOR_LOCKED"); // connect_wifi.c

                // Code for testing only
                // printf("CURRENTLY LAMP IS: Red\n");
                // portableSleep(3);
                // printf("CURRENTLY LAMP IS: Off\n");
                break;
        }

    closeConnection();
}
