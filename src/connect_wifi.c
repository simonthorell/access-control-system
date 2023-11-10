// wifi_module.c
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

/* Make sure to configure your MCU with static IP address "192.168.50.254" and port 23 */

#define PORT 23

// Global socket descriptor
int sock = -1;

// Establish a connection to the ESP8266
int establishConnection(const char* ipAddress) {
    struct sockaddr_in serv_addr;
    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ipAddress, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Attempt to connect to the ESP8266
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // printf("Connected to the ESP8266 at %s\n", ipAddress);
    return 0;
}

#define BUFFER_SIZE 1024
#define TIMEOUT_SECONDS 10

// Write a command to the ESP8266
void wifiWrite(const char* command) {
    if (sock != -1) {
        send(sock, command, strlen(command), 0);
        // printf("Command sent: %s\n", command);

        // Set up the timeout for recv
        struct timeval tv;
        tv.tv_sec = TIMEOUT_SECONDS;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        int bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0) {
            printf("Response: %s\n", buffer);
        } else if (bytesReceived == 0) {
            printf("Connection closed by the ESP8266.\n");
        } else {
            printf("No response within the timeout period or error in receiving.\n");
        }
    } else {
        printf("Connection not established.\n");
    }
}

// Close the connection
void closeConnection(void) {
    if (sock != -1) {
        close(sock);
        sock = -1;
        // printf("Connection closed.\n");
    }
}


