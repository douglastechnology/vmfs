#include <stdio.h>
#include <stdlib.h>
#include "xxhash.h"
#include "lz4.h"
#include "aes.h"

void write_block(const char *block)
{
	char *compressed = malloc(1048576);
	int compressed_size = LZ4_compress_default(block, compressed, 1048576, 1048576);

	unsigned char *key = (unsigned char *)"11111111111111111111111111111111";
	unsigned char *iv = (unsigned char *)"22222222222222222";
	unsigned char *encrypted = malloc(1048576);
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	int encrypted_size = encrypt((unsigned char *)compressed, compressed_size, key, iv, encrypted);

	char *path = malloc(100);
	unsigned long long hash = XXH64(block, 1048576, 0);
	sprintf(path, "/root/%llx", hash);

	FILE *fp = fopen(path, "w");
	size_t siz = fwrite(encrypted, 1, encrypted_size, fp);
	fclose(fp);

	free(compressed);
	free(encrypted);
	free(path);
}

void read_block(char *block)
{
	unsigned char *key = (unsigned char *)"11111111111111111111111111111111";
	unsigned char *iv = (unsigned char *)"22222222222222222";
	unsigned char *encrypted = malloc(1048576);
	FILE *fp = fopen("/root/25e9982ae4f7e71f", "r");
	size_t encrypted_size = fread(encrypted, 1, 1048576, fp);
	fclose(fp);
	
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	char *compressed = malloc(1048576);
	int compressed_size = decrypt(encrypted, encrypted_size, key, iv, (unsigned char *)compressed);

	LZ4_decompress_safe(compressed, block, compressed_size, 1048576);

	free(compressed);
	free(encrypted);
}
