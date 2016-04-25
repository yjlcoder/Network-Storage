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

using namespace std;
const int RSAKEYL = 128;
const int AESKEYL = 16;

struct SafeInfo{
	AES_KEY key;
	RSA * Rsa;
    int clientInit(unsigned char PublicKey[RSAKEYL]){
        Rsa = RSA_generate_key(RSAKEYL, RSA_F4, NULL, NULL);
		unsigned char *PKey = PublicKey;
		int PublicKeyLen = i2d_RSAPublicKey(Rsa, &PKey);
		return PublicKeyLen;
	}
	
	int clientKeySet(const unsigned char cipherText[AESKEYL]) {
		unsigned char clearText[AESKEYL]; 
				
		RSA_private_decrypt(AESKEYL, (const unsigned char*)cipherText, clearText, Rsa, RSA_NO_PADDING);
		AES_set_encrypt_key(clearText, AESKEYL*8, &key);

		RSA_free(Rsa);

		puts("mi");
		return 0;
	}

	int serviceKeySet(const int PublicKeyLen, unsigned char PublicKey[], unsigned char cipherText[AESKEYL]){
		unsigned char * PKey = PublicKey;
		Rsa = d2i_RSAPublicKey(NULL, (const unsigned char**)&PKey, PublicKeyLen);
		unsigned char clearText[AESKEYL]; 
		srand(time(0) + getpid());
		for (int i = 0; i < AESKEYL; ++i) {
			clearText[i] = rand()&0xff;
		}
		RSA_public_encrypt(AESKEYL, (const unsigned char*)clearText, cipherText, Rsa, RSA_NO_PADDING); 
		AES_set_decrypt_key(clearText, AESKEYL*8, &key);
		RSA_free(Rsa);
		return 0;
	}


	void decrypt(unsigned char *encrypt_string, char **decrypt_string, int len){
		unsigned char iv[AES_BLOCK_SIZE]; // init vector
		*decrypt_string = (char*)calloc(len, sizeof(char));
		if (*encrypt_string == 0) {
			fprintf(stderr, "Unable to allocate memory for encrypt_stringn");
			exit(-1);
		}	
		for (int i = 0; i< AES_BLOCK_SIZE; ++i) {
			iv[i] = 0;
		}
		AES_cbc_encrypt(encrypt_string, (unsigned char *)*decrypt_string, len, &key, iv, AES_DECRYPT);
	}

	int encrypt(char *input_string, unsigned char **encrypt_string) {
		unsigned char iv[AES_BLOCK_SIZE];
		unsigned int len = 0;
		if ((strlen(input_string) + 1) % AES_BLOCK_SIZE == 0) {
			len = strlen(input_string) + 1;
		} else {
			len = ((strlen(input_string) + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
		}

		for (int i = 0; i < AES_BLOCK_SIZE; ++i) {
			iv[i] = 0;
		}

		*encrypt_string = (unsigned char*)calloc(len, sizeof(unsigned char)); 
		if (*encrypt_string == 0) {
			fprintf(stderr, "Unable to allocate memory for encrypt_stringn");
			exit(-1);
		}

		AES_cbc_encrypt((unsigned char *)input_string, *encrypt_string, len, &key, iv, AES_ENCRYPT);
		return len;
	}

};

int main(){
	SafeInfo client, service;
	unsigned char buff[512];
	int Len = 0;
	Len = client.clientInit(buff);
	unsigned char data[16];
	service.serviceKeySet(Len, buff, data);


	puts("check0");
	client.clientKeySet(data);
	puts("check1");

	char info[256] = "zhangsanlisiwangwu";
	unsigned char * eninfo;
	char * deinfo;
	int l;
	l = client.encrypt(info, &eninfo);
	
	service.decrypt(eninfo, &deinfo, l);


	cout << deinfo << endl;
	cout << info << endl;
	
	free(eninfo);
	free(deinfo);
	return 0;
}
