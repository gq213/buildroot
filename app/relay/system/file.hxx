#ifndef __FILE_H__
#define __FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

int read_from_file(const char *name, unsigned char *buf, int size);
int read_from_file_1(const char *name, unsigned char **buf, int *size);
int save_to_file(const char *name, unsigned char *buf, int size);
int file_delete(const char *path);
int file_creat_empty(const char *path);
int file_rename(const char *oldpath, const char *newpath);
int creat_dir(const char *path);

#ifdef __cplusplus
}
#endif

#endif
