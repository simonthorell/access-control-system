#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>

#include "status_messages.h"
#include "input_output.h"

int serialConnect(char *port) {
    printInfoMessage("Connecting to RFID reader (Arduino/ESP32) via serial port...");
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    int serial_port = open(port, O_RDWR);
    
    if (serial_port == -1) {
        printStatusMessage(ERROR_OPERATION_FAILED, "Could not connect to RFID reader on serial port '%s' (check settings in admin menu!)", port);
        printSimulationMessage("Running RFID reader simulation mode");
        return ERROR_OPERATION_FAILED; // Return error for handling in caller
    } else {
        printStatusMessage(SUCCESS, "Connected to RFID reader on serial port %s", port);
        return serial_port;
    }
}

void serialDisconnect(int serial_port) {
    printInfoMessage("Disconnecting from RFID reader (Arduino/ESP32) via serial port...");
    close(serial_port);
    printStatusMessage(SUCCESS, "Disconnected from RFID reader on serial port");
}

char* serialRead(int serial_port, int timeout) {
    // Set the serial port to non-blocking
    int flags = fcntl(serial_port, F_GETFL, 0);
    fcntl(serial_port, F_SETFL, flags | O_NONBLOCK);

    // Allocate memory for read buffer
    char *read_buf = (char *)malloc(256 * sizeof(char));
    if (!read_buf) {
        printf("Error allocating memory.\n");
        return NULL;
    }
    memset(read_buf, '\0', 256);

    int num_bytes;
    time_t start_time = time(NULL);

    // Loop until data is read or timeout
    while (1) {
        num_bytes = read(serial_port, read_buf, 255);

        if (num_bytes > 0) {
            // Data read
            break;
        } else if (num_bytes < 0 && errno != EAGAIN) {
            // An error occurred
            printf("Error reading: %s\n", strerror(errno));
            free(read_buf);
            return NULL;
        }

        // Check for timeout
        if (time(NULL) - start_time > timeout) {
            // printf("Read timeout.\n");
            free(read_buf);
            return NULL;
        }

        // Sleep for 1 second
        sleep(1);
    }

    // Reallocate the buffer to the actual data size
    char *actual_data = realloc(read_buf, num_bytes + 1);
    if (!actual_data) {
        printf("Error reallocating memory.\n");
        free(read_buf);
        return NULL;
    }

    actual_data[num_bytes] = '\0'; // Null-terminate the string
    return actual_data;
}
