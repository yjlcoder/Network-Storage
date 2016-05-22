#include "RSA_AES.h"

using namespace std;

SafeInfo::SafeInfo(){
	Rsa = NULL;
	fp = NULL;
}

int SafeInfo::clientInit(unsigned char PublicKey[RSAKEYL]){
	Rsa = RSA_generate_key(RSAKEYL, RSA_F4, NULL, NULL);
	unsigned char *PKey = PublicKey;
	int PublicKeyLen = i2d_RSAPublicKey(Rsa, &PKey);
	return PublicKeyLen;
}

int SafeInfo::clientKeySet(const unsigned char cipherText[AESKEYL]) {
	unsigned char clearText[AESKEYL]; 
			
	RSA_private_decrypt(AESKEYL, (const unsigned char*)cipherText, clearText, Rsa, RSA_NO_PADDING);

	if (AES_set_decrypt_key(clearText, AESKEYL*8, &deKey) < 0) {
		if (fp != NULL)
			fprintf(fp, "Unable to set decryption key in AESn ------ %s_%D", __FILE__, __LINE__);
		exit(-1);
	}
	if (AES_set_encrypt_key(clearText, AESKEYL*8, &enKey) < 0) {
		if (fp != NULL)
			fprintf(fp, "Unable to set decryption key in AESn ------ %s_%D", __FILE__, __LINE__);
		exit(-1);
	}


	RSA_free(Rsa);
	return 0;
}

int SafeInfo::serviceKeySet(const int PublicKeyLen, unsigned char PublicKey[], unsigned char cipherText[AESKEYL]){
	unsigned char * PKey = PublicKey;
	Rsa = d2i_RSAPublicKey(NULL, (const unsigned char**)&PKey, PublicKeyLen);
	unsigned char clearText[AESKEYL]; 
	for (int i = 0; i < AESKEYL; ++i) {
		clearText[i] = rand()&0x7f;
	}
	RSA_public_encrypt(AESKEYL, (const unsigned char*)clearText, cipherText, Rsa, RSA_NO_PADDING); 
	if (AES_set_decrypt_key(clearText, AESKEYL*8, &deKey) < 0) {
		if (fp != NULL) fprintf(fp, "Unable to set decryption key in AESn ------ %s_%D", __FILE__, __LINE__);
		exit(-1);
	}
	if (AES_set_encrypt_key(clearText, AESKEYL*8, &enKey) < 0) {
		if (fp != NULL) fprintf(fp, "Unable to set decryption key in AESn ------ %s_%D", __FILE__, __LINE__);
		exit(-1);
	}
	RSA_free(Rsa);
	return 0;
}

void SafeInfo::decrypt(void *encrypt_string, void *decrypt_string, const int & len){
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0, sizeof(iv));
	AES_cbc_encrypt((unsigned char *)encrypt_string, (unsigned char *)decrypt_string, len, &deKey, iv, AES_DECRYPT);
}

int SafeInfo::encrypt(void * input_string, void * encrypt_string, const int & data_size) {
	unsigned char iv[AES_BLOCK_SIZE];
	unsigned int len = (((data_size - 1) >> 4) + 1) << 4;
	memset(iv, 0, sizeof(iv));
	AES_cbc_encrypt((unsigned char *)input_string, (unsigned char *)encrypt_string, len, &enKey, iv, AES_ENCRYPT);
	return len;
}

