#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H


//XOR encyrption mask
#define 	MASK			0xd3

// encrypts function with the given name
int encryptFunction(char* fnName);

// encrypts range of memory addresses
int encryptRange(long start, long end);

// decrypts a range of memory addresses
int decryptRange(unsigned int start, unsigned int end, int* decrypted);

#endif
