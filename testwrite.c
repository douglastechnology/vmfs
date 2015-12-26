#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rw.h"

int main(int argc, char *argv[])
{
	void *block = malloc(1048576);
	memset(block, 65, 1048576);
	write_block(block);
	memset(block, 0, 1048576);
	read_block(block);
	FILE *fp = fopen("/root/out", "w");
	size_t siz = fwrite(block, 1, 1048576, fp);
	fclose(fp);
	return 0;
}
