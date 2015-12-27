#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	char *path = malloc(100);
	sprintf(path, "/root/vm/%s", argv[1]);
	uint32_t siz = atol(argv[2]) * 32768;
	siz = htonl(siz);

	FILE *fp = fopen(path, "w");
	fwrite(&siz, sizeof(siz), 1, fp);
	const unsigned char *hash = (unsigned char *)"\x84\xd3\x17\x5e\xee\x20\x14\x8b\x0c\x2e\x2d\xe3\xad\x55\x6b\xbc";
	fwrite(hash, 16, 1, fp);
	fclose(fp);

        free(path);
        return 0;
}
