#include <string.h>
#include "input_output.h" // getPassword()
#include <stdio.h> // printf - REMOVE LATER ONCE TESING IS DONE

/* 
Testing to use bitwise operation XOR for encryption/decryption.
DO NOT USE FOR PRODUCTION CODE - THIS IS NOT SECURE AND ONLY FOR TESTING PURPOSES!
*/

#define PASSWORD_MAX_LENGTH 21

void encrypt(const char *input, char *output, const char *key) {
    int len = strlen(input);
    int keyLen = strlen(key);

    for (int i = 0; i < len; i++) {
        output[i] = input[i] ^ key[i % keyLen]; // Using XOR to encrypt
    }
    output[len] = '\0'; // Null-terminate the encrypted string
}

void decrypt(const char *input, char *output, const char *key) {
    encrypt(input, output, key); // XOR encryption is symmetric
}

int promptAdminPassword(void){
    char *adminPw = "admin";

    if (getPassword(adminPw, PASSWORD_MAX_LENGTH)) { // input_output.c
        return 1; // true
    } else {
        return 0; // false
    }
}

int changeAdminPassword(void){
    char newPassword[PASSWORD_MAX_LENGTH];
    char *key = "rockNroll";

    if (promptAdminPassword() == 0) {
        return -1; // exit and return to menu
    }

    if (getNewPassword(newPassword, PASSWORD_MAX_LENGTH) != 0) {
        return -1; // exit and return to menu
    }

    char encryptedPassword[PASSWORD_MAX_LENGTH];
    encrypt(newPassword, encryptedPassword, key);

    const char* filename = "password.txt";
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file to save encrypted password");
        return -1;
    }
    fprintf(file, "%s", encryptedPassword);
    fclose(file);

    return 0; // Password changed successfully
}
