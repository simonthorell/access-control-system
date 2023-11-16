#ifndef ENCRYPT_DECRYPT_H
#define ENCRYPT_DECRYPT_H

void encrypt(const char *input, char *output, const char *key);
void decrypt(const char *input, char *output, const char *key);

int promptAdminPassword(void);
int changeAdminPassword(void);

#endif // ENCRYPT_DECRYPT_H
