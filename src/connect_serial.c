#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>

int serialConnect(char *port) {
    printf("\033[33m* Attempting to connect to RFID reader (Arduino/ESP32) at serial port: %s\033[0m\n", port);
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    int serial_port = open(port, O_RDWR);
    
    if (serial_port == -1) {
        printf("\033[31m* Could not connect to RFID reader on serial port '%s' \033[0m\033[33m(Check serial port settings in admin menu!)\033[0m\n", port);
        return -1; // Return error for handling in caller
    } else {
        printf("\033[32m* Connected to RFID reader on serial port %s\033[0m\n", port);
        return serial_port;
    }
}

void serialDisconnect(int serial_port) {
    close(serial_port);
}

char* serialRead(int serial_port) {
    // Allocate memory for read buffer
    char *read_buf = (char *)malloc(256 * sizeof(char));
    if (!read_buf) {
        printf("Error allocating memory.\n");
        return NULL;
    }
    memset(read_buf, '\0', 256);

    // Read from the serial port
    int num_bytes = read(serial_port, read_buf, 255);

    if (num_bytes < 0) {
        printf("Error reading: %s\n", strerror(errno));
        free(read_buf);
        return NULL;
    }

    if (num_bytes == 0) {
        free(read_buf);
        return NULL;
    }

    // Reallocate the buffer to the actual data size
    char *actual_data = realloc(read_buf, num_bytes + 1);
    if (!actual_data) {
        printf("Error reallocating memory.\n");
        free(read_buf);
        return NULL;
    }

    actual_data[num_bytes] = '\0'; // Null-terminate the string
    // printf("Read %i bytes. Received message: %s\n", num_bytes, actual_data);
    return actual_data;
}
