#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rw.h"

int main(int argc, char *argv[])
{
	void *block = malloc(32768);
	memset(block, 65, 32768);
	write_block(block);
	memset(block, 0, 32768);
	read_block(block);
	FILE *fp = fopen("/root/out", "w");
	size_t siz = fwrite(block, 1, 32768, fp);
	fclose(fp);
	return 0;
}
