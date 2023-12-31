#include "door_control.h"
#include <stdio.h>
#include "util_sleep.h"
#include "connect_tcp_ip.h" // Connect to Wemos D1 Mini controlling door lock
#include "input_output.h" // printInfoMessage

// Set initial value of global variable to door locked.
int doorStatus = DOOR_LOCKED;

// Open door from admin menu
void remoteOpenDoor(void) {
    printInfoMessage("Remote unlocking door...");
    lockUnlockMechanism(DOOR_UNLOCKED);
    // Confirm that MCU door controller executed command (will only run if NOT in simulation mode).
    if (sock != -1) {
        printf("Door successfully unlocked!\n");
    }
}

// Sends signal to MCU to unlock the door
void lockUnlockMechanism(int doorLock) {
    int result;
    switch (doorLock) {
        case DOOR_UNLOCKED:
            // Code to send signal to MCU to unlock door => GREEN LED
            if (sock != -1) {
                result = wifiWrite("DOOR_UNLOCKED\r"); // connect_wifi.c
                if (result == 0) { // check if MCU door controller executed command
                    doorStatus = DOOR_UNLOCKED;
                    portableSleep(300); // Wait to avoid TCP/IP package loss
                }
            } else {
                printf("\nCURRENTLY LAMP IS: \033[1;32mGreen\033[0m \033[3;36m\t(simulation mode)\033[0m\n");
                doorStatus = DOOR_UNLOCKED;
                portableSleep(3000); // Wait to simulate door lock/unlock
            }
            lockUnlockMechanism(DOOR_LOCKED);
            break;
        case DOOR_LOCKED:
            // Code to send signal to MCU to lock door => RED LED
            if (sock != -1) {
                result = wifiWrite("DOOR_LOCKED\r"); // connect_wifi.c
                if (result == 0) {
                    doorStatus = DOOR_LOCKED;
                }
            } else {
                // Solving bug that occurs with Sleep in simulation mode
                if (doorStatus == DOOR_LOCKED) {
                    printf("\n");
                    portableSleep(3000);
                }
                printf("CURRENTLY LAMP IS: \033[1;31mRed\033[0m \033[3;36m\t\t(simulation mode)\033[0m\n");
                doorStatus = DOOR_LOCKED;
                portableSleep(3000); // Wait to simulate door lock/unlock
                printf("CURRENTLY LAMP IS: \033[1;90mOff\033[0m \033[3;36m\t\t(simulation mode)\033[0m\n");
                
            }   
            break;
    }
}
