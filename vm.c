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
	const unsigned char *hash = (unsigned char *)"\xfe\xbf\x04\xf4\x8e\xc8\xe2\x68\xad\x3f\xf9\x9f\x6a\x18\xff\xaf";
	fwrite(hash, 16, 1, fp);
	fclose(fp);

        free(path);
        return 0;
}
