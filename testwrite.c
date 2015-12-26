#include <stdlib.h>
#include <string.h>
#include "rw.h"

int main(int argc, char *argv[])
{
	void *block = malloc(1048576);
	memset(block, 65, 1048576);
	write_block(block);
	return 0;
}
