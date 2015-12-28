#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rw.h"

int main(int argc, char *argv[])
{
	unsigned char *hash = malloc(16);
	void *block = malloc(32768);
	memset(block, 0, 32768);
	write_block(block, hash);
	for (int i=0; i<32768; i+=16)
		memcpy(block + i, hash, 16);
	write_block(block, hash);
	for (int i=0; i<32768; i+=16)
		memcpy(block + i, hash, 16);
	write_block(block, hash);
	for (int i=0; i<32768; i+=16)
		memcpy(block + i, hash, 16);
	write_block(block, hash);
	free(block);
	free(hash);
	return 0;
}
