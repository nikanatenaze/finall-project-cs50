
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>
#include <stdbool.h>

char *fs_getcwd(char *buffer, size_t size);

int fs_chdir(const char *path);

int fs_list(const char *path);

int fs_create_dir(const char *path);

int fs_remove_dir(const char *path);

bool is_valid_directory(const char *path);

#endif