#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // errno, EINPROGRESS, EWOULDBLOCK
#include <fcntl.h> // fcntl

#include "connect_tcp_ip.h"
#include "status_messages.h"
#include "input_output.h"

// Global socket descriptor
int sock = -1;

int tcpConnect(char *ipAddress) {
    printInfoMessage("Attempting to connect to wireless door controller (ESP8266EX MCU) at IP address: %s", ipAddress);
    int connected = establishConnection(ipAddress);
    if (connected == SUCCESS) {
        printStatusMessage(SUCCESS, "Connected to door controller MCU via TCP/IP socket: %d", sock);
    } else {
        printStatusMessage(ERROR_OPERATION_FAILED, "Failed to connect to door controller MCU via TCP/IP socket.");
        printSimulationMessage("Running door controller in simulation mode...");
    }

    return sock;
}

// int establishConnection(const char* ipAddress, int timeoutSec) {
int establishConnection(const char* ipAddress) {
    int timeoutSec = 2; // 2 seconds timeout
    struct sockaddr_in serv_addr;

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Set the socket to non-blocking mode
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ipAddress, &serv_addr.sin_addr) <= 0) {
        // printf("\nInvalid address/ Address not supported \n");
        close(sock);
        sock = -1;
        return -1;
    }

    // Initiate connection (non-blocking)
    int result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (result < 0 && errno != EINPROGRESS) {
        // printf("\nConnection Failed immediately \n");
        close(sock);
        sock = -1;
        return -1;
    }

    if (result == 0) {
        // Connection succeeded immediately
        fcntl(sock, F_SETFL, flags); // Restore blocking mode
        return sock;
    }

    // Wait for the connection to succeed
    fd_set writefds;
    struct timeval tv;
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    tv.tv_sec = timeoutSec;
    tv.tv_usec = 0;

    result = select(sock + 1, NULL, &writefds, NULL, &tv);
    if (result > 0) {
        // Check if there was an error with the socket
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            // printf("Connection Established\n");
            fcntl(sock, F_SETFL, flags); // Restore blocking mode
            return sock;
        } else {
            // printf("Connection Error: %s\n", strerror(so_error));
        }
    } else if (result == 0) {
        // printf("Connection Timed Out\n");
    } else {
        // printf("Error on Select: %s\n", strerror(errno));
    }

    close(sock);
    sock = -1;
    return -1;
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
