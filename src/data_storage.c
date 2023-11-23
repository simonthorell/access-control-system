#include "data_storage.h"
#include <stdio.h>
#include <stdlib.h>             // malloc, free
#include <string.h>
#include <time.h>
#include "status_messages.h"    // custom error/success handling
#include "input_output.h"       // custom input/output functions (printMessage etc.)

int saveAccessCards(accessCard *pAccessCards, size_t cardCount) {
    printInfoMessage("Saving %zu access cards from memory location: %p to file 'access_cards.csv'...", cardCount, (void *)pAccessCards);
    FILE *file = fopen("access_cards.csv", "w");
    if (!file) return ERROR_FILE_NOT_FOUND; // Could not open file for writing.

    // Write header line to CSV file
    fprintf(file, "CardNumber,Status,DateCreated\n");

    // Iterate over each accessCard and write to file
    for (size_t i = 0; i < cardCount; i++) {
        const char* statusStr = pAccessCards[i].cardAccess == ACCESS ? "ACCESS" : "NO_ACCESS";
        
        // Write the CSV line
        if (fprintf(file, "%lu,%s,%ld\n", 
                    pAccessCards[i].cardNumber, 
                    statusStr, 
                    pAccessCards[i].dateCreated) < 0) {
            fclose(file);
            return -2; // Writing to file failed
        }
    }

    fclose(file); // Close the file after writing
    return SUCCESS; // Success
}

// NOTE! Caller is responsible for freeing the memory allocated by this function!
accessCard* retrieveAccessCards(size_t *cardsMallocated, size_t *cardCount) {
    FILE *file = fopen("access_cards.csv", "r");
    if (!file) return NULL;

    char line[256]; // buffer for reading a line from the .csv file. Make smaller?? 

    // Allocate space for the array in the heap
    accessCard *accessCards = malloc(*cardsMallocated * sizeof(accessCard));

    // Skip header line
    fgets(line, sizeof(line), file);

    // Check that accessCards is not NULL && check that successfully read a line from the file.
    while (accessCards && fgets(line, sizeof(line), file)) {
        // CHeck if we need to resize the array, if so DOUBLE the capacity. 
        if (*cardCount >= *cardsMallocated) {
            *cardsMallocated *= 2;
            accessCard *temp = realloc(accessCards, *cardsMallocated * sizeof(accessCard));
            if (!temp) break;
            accessCards = temp;
        }

        // Temp variables to store the parsed values
        char statusStr[20];
        unsigned long int cardNumber;
        time_t dateCreated;

        // Read the CSV line and parse it
        if (sscanf(line, "%lu,%19[^,],%ld", &cardNumber, statusStr, &dateCreated) == 3) {
            accessCards[*cardCount].cardNumber = cardNumber;
            accessCards[*cardCount].cardAccess = strcmp(statusStr, "ACCESS") == 0 ? ACCESS : NO_ACCESS;
            accessCards[*cardCount].dateCreated = dateCreated;
            (*cardCount)++;
        }
    }

    fclose(file);
    return accessCards; // Caller is responsible for freeing the memory!!!
}

int saveConfig(const char* filename, Configuration *config) {
    FILE *file;
    char line[256];
    char temp_rfid_win32[50] = "";
    char temp_rfid_linux[50] = "";
    char temp_rfid_mac[50] = "";

    // Open the file in read mode to fetch the current values
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return ERROR_FILE_NOT_FOUND;
    }

    while (fgets(line, sizeof(line), file)) {
        char key[256], value[256];

        if (sscanf(line, "%s = %s", key, value) == 2) {
            if (strcmp(key, "rfid_serial_port_win32") == 0) {
                strcpy(temp_rfid_win32, value);
            } else if (strcmp(key, "rfid_serial_port_linux") == 0) {
                strcpy(temp_rfid_linux, value);
            } else if (strcmp(key, "rfid_serial_port_mac") == 0) {
                strcpy(temp_rfid_mac, value);
            }
        }
    }
    fclose(file);

    // Update the appropriate value for the current OS
    #ifdef _WIN32
        strcpy(temp_rfid_win32, config->rfid_serial_port);
    #elif defined(__linux__)
        strcpy(temp_rfid_linux, config->rfid_serial_port);
    #elif defined(__APPLE__)
        strcpy(temp_rfid_mac, config->rfid_serial_port);
    #else // Default to linux
        strcpy(temp_rfid_linux, config->rfid_serial_port);
    #endif

    // Now open the file in write mode to update the values
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return ERROR_FILE_NOT_FOUND;
    }

    // Write the updated values
    fprintf(file, "[Arduino_RFID_Reader]\n");
    fprintf(file, "rfid_serial_port_win32 = %s\n", temp_rfid_win32);
    fprintf(file, "rfid_serial_port_linux = %s\n", temp_rfid_linux);
    fprintf(file, "rfid_serial_port_mac = %s\n", temp_rfid_mac);

    fprintf(file, "[ESP8266_Door_Control]\n");
    fprintf(file, "door_ip_address = %s\n", config->door_ip_address);
    fprintf(file, "door_tcp_port = %d\n", config->door_tcp_port);

    fclose(file);

    return SUCCESS;
}

Configuration* readConfig(const char* filename) {
    const int MAX_LINE_LENGTH = 256;
    FILE *file;
    char line[MAX_LINE_LENGTH];
    Configuration *config = malloc(sizeof(Configuration));

    // Ensure the memory allocation was successful
    if (config == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Initialize the struct to default values
    strcpy(config->rfid_serial_port, "");
    strcpy(config->door_ip_address, "");
    config->door_tcp_port = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        free(config); // Free the allocated memory if file opening fails
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        line[strcspn(line, "\n")] = 0; // Remove newline character

        if (strstr(line, "rfid_serial_port") != NULL) {
            #ifdef _WIN32
                sscanf(line, "rfid_serial_port_win32 = %s", config->rfid_serial_port);
            #elif defined(__linux__)
                sscanf(line, "rfid_serial_port_linux = %s", config->rfid_serial_port);
            #elif defined(__APPLE__)
                sscanf(line, "rfid_serial_port_mac = %s", config->rfid_serial_port);
            #else // Default to linux
                sscanf(line, "rfid_serial_port_linux = %s", config->rfid_serial_port);
            #endif
        } else if (strstr(line, "door_ip_address") != NULL) {
            sscanf(line, "door_ip_address = %s", config->door_ip_address);
        } else if (strstr(line, "door_tcp_port") != NULL) {
            sscanf(line, "door_tcp_port = %d", &config->door_tcp_port);
        }
    }

    fclose(file);
    return config;
}
