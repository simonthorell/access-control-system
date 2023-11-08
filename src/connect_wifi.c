#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>  // Include for close function

#define PORT 80
#define SERVER_IP "your_wemos_ip_address"  // Replace with the IP address of your Wemos

int wifiWrite(const char *command) {
    int sock;
    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        return -1;
    }
    
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to Arduino server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return -1;
    }

    // Send the command
    if (send(sock, command, strlen(command), 0) < 0) {
        perror("Send failed");
        return -1;
    }

    // Optionally receive a response back
    char server_reply[2000];
    if (recv(sock, server_reply, sizeof(server_reply), 0) < 0) {
        puts("Recv failed");
    }
    
    printf("Arduino reply: %s\n", server_reply);

    // Close the socket
    close(sock);

    return 0;
}
