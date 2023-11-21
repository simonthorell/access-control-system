#ifndef STATUS_MESSAGES_H
#define STATUS_MESSAGES_H

// enum for return codes. Success 0 and error codes < 0
typedef enum {
    // Success codes
    SUCCESS = 0,                        // General success

    // Error codes
    ERROR_GENERAL = 1,                  // General error
    ERROR_INVALID_INPUT = 2,            // Invalid input
    ERROR_FILE_NOT_FOUND = 3,           // File or resource not found
    ERROR_NETWORK_ISSUE = 4,            // Network-related error
    ERROR_PERMISSION_DENIED = 5,        // Permission denied
    ERROR_OPERATION_FAILED = 6,         // Operation failed
    ERROR_RESOURCE_BUSY = 7,            // Resource busy or locked
    ERROR_TIMEOUT = 8,                  // Operation timed out
    ERROR_OUT_OF_MEMORY = 9,            // Out of memory

} ReturnCode;

void printStatusMessage(ReturnCode code, const char* customFormat, ...);
const char* getDefaultStatusMessage(ReturnCode code);

#endif // STATUS_MESSAGES_H
