#ifndef CARD_READER_H
#define CARD_READER_H

void fakeTestScanCard(accessCard *pAccessCards, size_t *pCardCount);
unsigned int rfidReading(accessCard *pAccessCards, size_t *pCardCount, int serial_port);
int cardAuthentication(accessCard *pAccessCards, size_t *pCardCount, unsigned int cardNumber);

// Convert RFID card number to unsigned int and vice versa.
unsigned int hexToUint(char *hexString);
char *uintToHex(unsigned int cardNumber);

#endif
