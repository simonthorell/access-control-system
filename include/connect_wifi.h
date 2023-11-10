#ifndef CONNECT_WIFI_H
#define CONNECT_WIFI_H

int establishConnection(const char* ipAddress);
int wifiWrite(const char *command);
void closeConnection(void);

#endif
