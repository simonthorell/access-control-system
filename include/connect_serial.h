#ifndef CONNECT_SERIAL_H
#define CONNECT_SERIAL_H

int serialConnect(char *port);
void serialDisconnect(int serial_port);
char* serialRead(int serial_port);

#endif
