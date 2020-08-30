#include "encryptor.h"
#include <stdio.h>


#define		ENCRYPTED_FUNCTION		static int isFunctionAlreadyDecrypted = 0;	\
						decryptRange(0, 1, &isFunctionAlreadyDecrypted); 



int decryptRange(unsigned int start, unsigned int end, int* decrypted) {
	if (*decrypted == 1) {
		printf("Function already decrypted");
		return 0;
	}

	*decrypted = 1;
	printf("decrypting range %x   %x\n", start, end);
	char* byteToDecrypt = (char*) start;

	for (int i = 0; i <= end - start; ++i) {
		byteToDecrypt[i] = (byteToDecrypt[i] ^ MASK);
	}

	return 0;
}

int testFunction1(char* fnName) {
	ENCRYPTED_FUNCTION
	printf("Hello World!\n");
	int x = 2 + 2;
	int y = x * 3;
	return y;
}

void testFunction2() {
	ENCRYPTED_FUNCTION
	printf("Back from dead!\n");
	int x = 0;
	int y = 0;
	x * y;
}

int main(int argc, char** argv) {
	testFunction1("hello");
	testFunction2();
	return 0;
}
