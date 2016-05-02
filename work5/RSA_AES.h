#ifndef _RSA_AES_H_
#define _RSA_AES_H_
#include <cstring>
#include <cstdlib>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <unistd.h>
#include <memory.h>
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <iostream>

static const int RSAKEYL = 128;
static const int AESKEYL = 16;

class SafeInfo{
	AES_KEY deKey;
	AES_KEY enKey;
	RSA * Rsa;

public:
	FILE * fp;

	SafeInfo();
    int clientInit(unsigned char PublicKey[RSAKEYL]);
	int clientKeySet(const unsigned char cipherText[AESKEYL]);
	int serviceKeySet(const int PublicKeyLen, unsigned char PublicKey[], unsigned char cipherText[AESKEYL]);
	void decrypt(void *encrypt_string, void *decrypt_string, const int & len);
	int encrypt(void * input_string, void * encrypt_string, const int & data_size);
};

#endif
