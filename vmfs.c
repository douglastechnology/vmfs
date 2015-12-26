#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static const char *vmfs_path = "/vm";

static int vmfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, vmfs_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1073741824;
	} else
		res = -ENOENT;

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
	filler(buf, vmfs_path + 1, NULL, 0);

	return 0;
}

static int vmfs_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, vmfs_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static size_t fs_read(char **bufp, size_t size, off_t offset)
{
	FILE *fp = fopen("/root/raw101.1M", "r");
	fseek(fp, offset, SEEK_SET);
	size_t siz = fread(*bufp, 1, size, fp);
	fclose(fp);
	*bufp += siz;

	return siz;
}

static int vmfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	(void) fi;
	size_t siz = size;

	FILE *fp = fopen("/root/vmfs/log", "a");
	fprintf(fp, "%i %i", (int)size, (int)offset);

	if (offset < 1073741824) {
		if (offset + siz > 1073741824)
			siz = 1073741824 - offset;
                while (offset >= 1048576)
			offset -= 1048576;
		if (offset + siz <= 1048576) {
			fprintf(fp, " %i", (int)fs_read(&buf, siz, offset));
		} else {
			fprintf(fp, " %i", (int)fs_read(&buf, 1048576 - offset, offset));
			siz -= (1048576 - offset);
			while (siz > 1048576) {
				fprintf(fp, " %i", (int)fs_read(&buf, 1048576, 0));
				siz -= 1048576;
			}
			if (siz > 0) {
				fprintf(fp, " %i", (int)fs_read(&buf, siz, 0));
			}
		}
	} else
		size = 0;

	fprintf(fp, "\n");
	fclose(fp);

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