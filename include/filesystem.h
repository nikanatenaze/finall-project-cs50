
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>

char *fs_getcwd(char *buffer, size_t size);

int fs_chdir(const char *path);

int fs_list(const char *path);

int fs_create_dir(const char *path);

int fs_remove_dir(const char *path);

#endif