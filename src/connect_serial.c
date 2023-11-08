#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>

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
    return actual_data;
}
