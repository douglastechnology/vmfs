#include <stdio.h>
#include <stdlib.h>
#include "murmur3.h"
#include "lz4.h"
#include "aes.h"

void write_block(const char *block, unsigned char *hash)
{
	char *compressed = malloc(32768);
	int compressed_size = LZ4_compress_default(block, compressed, 32768, 32768);

	unsigned char *key = (unsigned char *)"11111111111111111111111111111111";
	unsigned char *iv = (unsigned char *)"22222222222222222";
	unsigned char *encrypted = malloc(32768);
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	int encrypted_size = encrypt((unsigned char *)compressed, compressed_size, key, iv, encrypted);

	char *path = malloc(100);
	MurmurHash3_x64_128(block, 32768, 0, hash);
	sprintf(path, "/root/hash/%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]);

	FILE *fp = fopen(path, "w");
	size_t siz = fwrite(encrypted, 1, encrypted_size, fp);
	fclose(fp);

	free(compressed);
	free(encrypted);
	free(path);
}

void read_block(const unsigned char *hash, char *block)
{
	unsigned char *key = (unsigned char *)"11111111111111111111111111111111";
	unsigned char *iv = (unsigned char *)"22222222222222222";
	unsigned char *encrypted = malloc(32768);

	char *path = malloc(100);
	sprintf(path, "/root/hash/%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]);

	FILE *fp = fopen(path, "r");
	size_t encrypted_size = fread(encrypted, 1, 32768, fp);
	fclose(fp);
	
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	char *compressed = malloc(32768);
	int compressed_size = decrypt(encrypted, encrypted_size, key, iv, (unsigned char *)compressed);

	LZ4_decompress_safe(compressed, block, compressed_size, 32768);

	free(compressed);
	free(encrypted);
	free(path);
}
