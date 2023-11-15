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
}

// Sends signal to MCU to unlock the door
void lockUnlockMechanism(int doorLock) {
    int result;
    switch (doorLock) {
        case DOOR_UNLOCKED:
            // Code to send signal to MCU to unlock door => GREEN LED
            if (sock != -1) {
                result = wifiWrite("DOOR_UNLOCKED"); // connect_wifi.c
                if (result == 0) {
                    doorStatus = DOOR_UNLOCKED;
                    printf("Door unlocked!\n");
                }
            } else {
                printf("CURRENTLY LAMP IS: \033[1;32mGreen\033[0m \033[3;36m\t(simulation mode)\033[0m\n");
                portableSleep(3000); // Wait for 3 seconds to simulate door lock/unlock
                doorStatus = DOOR_UNLOCKED;
            }
            portableSleep(100); // Wait to time LED on door controller (otherwise not queing up correct on MCU).
            lockUnlockMechanism(DOOR_LOCKED);
            break;
        case DOOR_LOCKED:
            // Code to send signal to MCU to lock door => RED LED
            if (sock != -1) {
                result = wifiWrite("DOOR_LOCKED"); // connect_wifi.c
                if (result == 0) {
                    doorStatus = DOOR_LOCKED;
                    printf("Door locked!\n");
                }
            } else {
                printf("CURRENTLY LAMP IS: \033[1;31mRed\033[0m \033[3;36m\t\t(simulation mode)\033[0m\n");
                doorStatus = DOOR_LOCKED;
                portableSleep(3000); // Wait for 3 seconds to simulate door lock/unlock
                printf("CURRENTLY LAMP IS: \033[1;90mOff\033[0m \033[3;36m\t\t(simulation mode)\033[0m\n");
                
            }
            break;
    }
}
