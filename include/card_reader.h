#ifndef CARD_READER_H
#define CARD_READER_H

void fakeTestScanCard(accessCard *pAccessCards, size_t *pCardCount);
void rfidReading(void);
int cardAuthentication(accessCard *pAccessCards, size_t *pCardCount, int cardNumber);

#endif
