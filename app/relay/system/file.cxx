#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file.hxx"

int read_from_file(const char *name, unsigned char *buf, int size)
{
	FILE *fp;
	int ret;
	
	fp = fopen(name, "rb");
	if (fp == NULL) {
		printf("%s: Cannot open %s, errno=%d(%s)\n", __func__, name, errno, strerror(errno));
		return -1;
	}
	
	ret = fread(buf, 1, size, fp);
	
	fclose(fp);
	
	return ret;
}

int read_from_file_1(const char *name, unsigned char **buf, int *size)
{
	FILE *fp;
	int in_size = 0;
	unsigned char* in_data = NULL;
	
	fp = fopen(name, "rb");
	if (fp == NULL) {
		printf("%s: Cannot open %s, errno=%d(%s)\n", __func__, name, errno, strerror(errno));
		return -1;
	}
	
	fseek(fp, 0, SEEK_END);
	in_size = ftell(fp);
	if (in_size == 0) {
		printf("%s: file(%s) is empty!\n", __func__, name);
		fclose(fp);
		return -2;
	}
	
	in_data = (unsigned char *)malloc(in_size + 1);
	if (in_data == NULL) {
		printf("%s-%d: malloc fail\n", __func__, __LINE__);
		fclose(fp);
		return -1;
	}
	in_data[in_size] = '\0';
	
	rewind(fp);
	
	fread(in_data, 1, in_size, fp);
	
	fclose(fp);
	
	*buf = in_data;
	*size = in_size;
	
	return 0;
}

int save_to_file(const char *name, unsigned char *buf, int size)
{
	FILE *fp;
	int ret;

	fp = fopen(name, "w+b");
	if (fp == NULL) {
		printf("%s: Cannot open %s, errno=%d(%s)\n", __func__, name, errno, strerror(errno));
		return -1;
	}
	
	ret = fwrite(buf, 1, size, fp);
	
	fflush(fp);
	fsync(fileno(fp));

	fclose(fp);
	
	if (ret != size) {
		printf("save %s error!(%d,%d)\n", name, ret, size);
		return -1;
	}

	return 0;
}

int file_delete(const char *path)
{
	int ret = unlink(path);
	return ret;
}

int file_creat_empty(const char *path)
{
	int ret = creat(path, 0664);
	return ret;
}

int file_rename(const char *oldpath, const char *newpath)
{
	int ret = rename(oldpath, newpath);
	return ret;
}

int creat_dir(const char *path)
{
	if (access(path, F_OK) < 0) {
		if (mkdir(path, 0777) < 0) {
			printf("creat %s fail\n", path);
			return -1;
		}
	}
	
	return 0;
}
