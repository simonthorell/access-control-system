#ifndef CARD_READER_H
#define CARD_READER_H

void fakeTestScanCard(accessCard *pAccessCards, size_t *pCardCount);
unsigned long int rfidReading(volatile bool *runCardReaderThread, accessCard *pAccessCards, size_t *pCardCount, int serial_port);
int cardAuthentication(accessCard *pAccessCards, size_t *pCardCount, unsigned long int cardNumber);

// Convert RFID card number to unsigned int and vice versa.
unsigned long int hexToUint(char *hexString);
void uintToHex(unsigned long int cardNumber, char *cardNumberString, size_t bufferSize);

#endif
