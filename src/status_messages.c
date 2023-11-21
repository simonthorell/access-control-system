#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "status_messages.h"    // custom status codes enum
#include "input_output.h"       // printErrorMessage, printSuccessMessage

/* Example usage:
 *  printStatusMessage(SUCCESS, "Program started successfully");
 *  printStatusMessage(ERROR_GENERAL, NULL);
 *  printStatusMessage(ERROR_GENERAL, "/path/to/resource %d", 404);
*/

void printStatusMessage(ReturnCode code, const char* customFormat, ...) {
    va_list args;
    va_start(args, customFormat);

    char buffer[256];

    if (customFormat == NULL || strlen(customFormat) == 0) {
        // Use default message if custom format is empty or NULL
        snprintf(buffer, sizeof(buffer), "%s", getDefaultStatusMessage(code));
    } else {
        // Use custom format with variadic arguments
        vsnprintf(buffer, sizeof(buffer), customFormat, args);
    }
    if (code == 0) {
        printSuccessMessage(buffer);
    } else {
        printErrorMessage(buffer);
    }

    va_end(args);
}

// Set default messages for each ReturnCode
const char* getDefaultStatusMessage(ReturnCode code) {
    switch (code) {
        case SUCCESS:
            return "Operation completed successfully";
        case ERROR_GENERAL:
            return "A general error has occurred";
        case ERROR_INVALID_INPUT:
            return "Invalid input provided";
        case ERROR_FILE_NOT_FOUND:
            return "The requested file or resource was not found";
        case ERROR_NETWORK_ISSUE:
            return "A network-related error occurred";
        case ERROR_PERMISSION_DENIED:
            return "Permission denied for the requested operation";
        case ERROR_OPERATION_FAILED:
            return "The operation failed to complete successfully";
        case ERROR_RESOURCE_BUSY:
            return "The requested resource is busy or locked";
        case ERROR_TIMEOUT:
            return "The operation timed out";
        case ERROR_OUT_OF_MEMORY:
            return "Out of memory error encountered";
        // ... other error codes as needed ...
        default:
            return "Unknown error code";
    }
}
