#include <stdio.h>
#include <stdlib.h>
#include "xxhash.h"

void write_block(const char *block)
{
	char *path = malloc(100);
	unsigned long long hash = XXH64(block, 1048576, 0);
	sprintf(path, "/root/%llx", hash);
	FILE *fp = fopen(path, "w");
	size_t siz = fwrite(block, 1, 1048576, fp);
}
