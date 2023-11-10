#ifndef CONNECT_TCP_IP_H
#define CONNECT_TCP_IP_H

/* Make sure to configure your MCU with static IP address "192.168.50.254" and port 23 */
#define PORT 23

// Variables for sending and receiving data
#define BUFFER_SIZE 64 // (Recommended 1024) - 64 is enough for this project (DOOR_UNLOCKED = 14 bytes incl. NULL terminator)
#define TIMEOUT_SECONDS 10

// Declaration of global socket descriptor
extern int sock;

int establishConnection(const char* ipAddress);
int wifiWrite(const char *command);
void closeConnection(void);

#endif
