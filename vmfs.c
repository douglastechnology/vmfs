#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h> 
#include <fuse.h>

#include "rw.h"

static int vmfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}
	else
	{
		char *source_path = malloc(100);
		sprintf(source_path, "/root/vm/%s", path);

		FILE *fp = fopen(source_path, "r");
		uint32_t siz;
		fread(&siz, sizeof(siz), 1, fp);
		siz = ntohl(siz) * 32768;

		fclose(fp);
		free(source_path);

		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = siz;
	}

	return res;
}

static int vmfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	DIR *d;
	struct dirent *dir;
	d = opendir("/root/vm");
	if(d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG)
			{
				filler(buf, dir->d_name, NULL, 0);
			}
		}
		closedir(d);
	}

	return 0;
}

static int vmfs_open(const char *path, struct fuse_file_info *fi)
{
	//if (strcmp(path, vmfs_path) != 0)
	//	return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static void next_block(int *offset_128G, int *offset_64M, int *offset_32K, void *block_128G_hashes, void *block_64M_hashes, void *block_32K_hashes, void *block_leaf)
{
	(*offset_32K)++;
	if( *offset_32K >= 2048 )
	{
		(*offset_64M)++;
		if( *offset_64M >= 2048 )
		{
			(*offset_128G)++;
			read_block(block_128G_hashes + 16 * *offset_128G, block_64M_hashes);
		}
		read_block(block_64M_hashes + 16 * *offset_64M, block_32K_hashes);
	}
	read_block(block_32K_hashes + 16 * *offset_32K, block_leaf);
}

static int vmfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	(void) fi;
	size_t siz = size;

	//FILE *logfp = fopen("/root/vmfs.log", "a");

	char *source_path = malloc(100);
	sprintf(source_path, "/root/vm%s", path);
	FILE *fp = fopen(source_path, "r");
	uint32_t siztemp;
	fread(&siztemp, sizeof(siztemp), 1, fp);
	long long fsize = ntohl(siztemp) * 32768;
	unsigned char *hash_root = malloc(16);
	fread(hash_root, 16, 1, fp);
	fclose(fp);

	//fprintf(logfp, "%s filesize: %lld size: %lld offset: %llu", path, fsize, (long long)size, (long long)offset);

	int offset_128G = 0;
	int offset_64M = 0;
	int offset_32K = 0;

	void *block_128G_hashes = malloc(32768);
	void *block_64M_hashes = malloc(32768);
	void *block_32K_hashes = malloc(32768);
	void *block_leaf = malloc(32768);

	if (offset < fsize)
	{
		if (offset + siz > fsize)
			siz = fsize - offset;
		read_block(hash_root, block_128G_hashes);
                while (offset >= 137438953472)
		{
			offset -= 137438953472;
			offset_128G++;
		}

		read_block(block_128G_hashes + 16 * offset_128G, block_64M_hashes);
                while (offset >= 67108864)
		{
			offset -= 67108864;
			offset_64M++;
		}
		read_block(block_64M_hashes + 16 * offset_64M, block_32K_hashes);
                while (offset >= 32768)
		{
			offset -= 32768;
			offset_32K++;
		}
		read_block(block_32K_hashes + 16 * offset_32K, block_leaf);

		if (offset + siz <= 32768)
		{
			memcpy(buf, block_leaf + offset, siz);
			//fprintf(logfp, " %i", (int)siz);
		}
		else
		{
			memcpy(buf, block_leaf + offset, 32768 - (int)offset);
			buf += (32768 - offset);
			siz -= (32768 - offset);
			next_block(&offset_128G, &offset_64M, &offset_32K, block_128G_hashes, block_64M_hashes, block_32K_hashes, block_leaf);
			//fprintf(logfp, " %i", 32768 - (int)offset);

			while (siz > 32768)
			{
				memcpy(buf, block_leaf, 32768);
				buf += 32768;
				siz -= 32768;
				next_block(&offset_128G, &offset_64M, &offset_32K, block_128G_hashes, block_64M_hashes, block_32K_hashes, block_leaf);
				//fprintf(logfp, " %i", 32768);
			}
			if (siz > 0)
			{
				memcpy(buf, block_leaf, siz);
				//fprintf(logfp, " %i", (int)siz);
			}
		}
	}
	else
		size = 0;

	free(hash_root);
	free(block_128G_hashes);
	free(block_64M_hashes);
	free(block_32K_hashes);
	free(block_leaf);

	//fprintf(logfp, "\n");
	//fclose(logfp);

	return size;
}

static struct fuse_operations vmfs_oper = {
	.getattr	= vmfs_getattr,
	.readdir	= vmfs_readdir,
	.open		= vmfs_open,
	.read		= vmfs_read,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &vmfs_oper, NULL);
}
