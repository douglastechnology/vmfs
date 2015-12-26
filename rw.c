#include <stdio.h>
#include <stdlib.h>
#include "xxhash.h"
#include "lz4.h"

void write_block(const char *block)
{
	char *compressed = malloc(1048576);
	int compressed_size = LZ4_compress_default(block, compressed, 1048576, 1048576);

	char *path = malloc(100);
	unsigned long long hash = XXH64(block, 1048576, 0);
	sprintf(path, "/root/%llx", hash);

	FILE *fp = fopen(path, "w");
	size_t siz = fwrite(block, 1, compressed_size, fp);
}
