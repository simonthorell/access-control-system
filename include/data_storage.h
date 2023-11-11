#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "card_management.h" // Access to card struct and access enum

typedef struct {
    char rfid_serial_port[50];
    char door_ip_address[50];
    int door_tcp_port;
} Configuration;

// Load & Save access_cards.csv file to/from memory
int saveAccessCards(accessCard *pAccessCards, size_t cardCount);
accessCard* retrieveAccessCards(size_t *cardsMallocated, size_t *cardCount);

// TODO: Load & Save access_logs.json file to/from memory

// Load & Save config.ini file to/from memory
void saveConfig(const char* filename, Configuration *config);
Configuration* readConfig(const char* filename);

#endif
