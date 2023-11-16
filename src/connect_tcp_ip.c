#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // errno, EINPROGRESS, EWOULDBLOCK
#include <fcntl.h> // fcntl
#include "connect_tcp_ip.h"

// Global socket descriptor
int sock = -1;

int tcpConnect(char *ipAddress) {
    printf("\033[33m* Attempting to connect to wireless door controller (ESP8266EX MCU) at IP address: %s\033[0m\n", ipAddress);
    int connected = establishConnection(ipAddress);
    if (connected != -1) {
        printf("\033[32m* Connected to door controller MCU via TCP/IP socket: %d\033[0m\n", sock);
    } else {
        printf("\033[31m* Failed to connect to door controller...\033[0m\n");
        printf("\033[36m* Running door controller in simulation mode...\033[0m\n");
    }

    return sock;
}

// int establishConnection(const char* ipAddress) {
//     struct sockaddr_in serv_addr;
//     long arg;
//     fd_set myset;
//     struct timeval tv;
//     int result, so_error;
//     socklen_t len = sizeof(int);

//     // Create socket
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         printf("\n Socket creation error \n");
//         return -1;
//     }

//     // Set non-blocking 
//     arg = fcntl(sock, F_GETFL, NULL);
//     arg |= O_NONBLOCK;
//     fcntl(sock, F_SETFL, arg);

//     // Set up address structure
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(PORT);
//     if (inet_pton(AF_INET, ipAddress, &serv_addr.sin_addr) <= 0) {
//         printf("\nInvalid address/ Address not supported \n");
//         close(sock);
//         return -1;
//     }

//     // Try to connect with timeout
//     result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
//     if (result < 0) {
//         if (errno == EINPROGRESS) {
//             tv.tv_sec = 3; // 3 seconds timeout
//             tv.tv_usec = 0;
//             FD_ZERO(&myset);
//             FD_SET(sock, &myset);
//             if (select(sock + 1, NULL, &myset, NULL, &tv) > 0) {
//                 len = sizeof(int);
//                 getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)(&so_error), &len);
//                 if (so_error == 0) {
//                     printf("\033[32m* * Connection Successful\033[0m\n");
//                     close(sock);
//                     return 0;
//                 } else {
//                     printf("\033[31m* Connection Failed\033[0m\n");
//                 }
//             } else {
//                 printf("\033[31m* Timeout or Select Error\033[0m\n");
//             }
//         } else {
//             printf("\033[31m* Connection Failed\033[0m\n");
//         }
//     } else {
//         printf("\033[32m* * Connection Successful\033[0m\n");
//     }

//     close(sock);
//     sock = -1;
//     return -1;
// }


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

    return 0;
}

// Write a command to the ESP8266 microcontroller
int wifiWrite(const char* command) {
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
            // printf("Response: %s\n", buffer);
            return 0;
        } else if (bytesReceived == 0) {
            printf("Connection closed by the ESP8266.\n");
            return -1;
        } else {
            printf("No response within the timeout period or error in receiving.\n");
            return -1;
        }
    } else {
        printf("Connection not established.\n");
        return -1;
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
