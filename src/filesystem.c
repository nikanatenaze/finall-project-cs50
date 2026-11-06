#include "filesystem.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// optimised includes and definitions for windows
#ifdef _WIN32

#include <direct.h>
#include <windows.h>
#include <direct.h>

#define CREATE_DIR(path) _mkdir(path)
#define REMOVE_DIR(path) _rmdir(path)

// optimised includes and definitions for linux
#else

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#define REMOVE_DIR(path) rmdir(path)
#define CREATE_DIR(path) mkdir(path, 0755)

#endif

// gets current working dir
char *fs_getcwd(char *buffer, size_t size)
{
#ifdef _WIN32
    return _getcwd(buffer, (int)size);
#else
    return getcwd(buffer, size);
#endif
}

// changes current working dir
int fs_chdir(const char *path)
{
#ifdef _WIN32
    return _chdir(path);
#else
    return chdir(path);
#endif
}

// shows files in current dir
int fs_list(const char *path)
{
#ifdef _WIN32

    char search_path[4096];

    snprintf(search_path, sizeof(search_path), "%s\\*", path);

    WIN32_FIND_DATAA entry;
    HANDLE handle = FindFirstFileA(search_path, &entry);

    if (handle == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Unable to open directory.\n");
        return -1;
    }

    do
    {
        if (strcmp(entry.cFileName, ".") == 0 ||
            strcmp(entry.cFileName, "..") == 0)
        {
            continue;
        }

        if (entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("[DIR]  %s\n", entry.cFileName);
        }
        else
        {
            printf("[FILE] %s\n", entry.cFileName);
        }

    } while (FindNextFileA(handle, &entry));

    FindClose(handle);

#else

    DIR *directory = opendir(path);

    if (directory == NULL)
    {
        perror("Unable to open directory");
        return -1;
    }

    // linux file API directory
    struct dirent *entry;

    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char full_path[4096];

        int length = snprintf(
            full_path,
            sizeof(full_path),
            "%s/%s",
            path,
            entry->d_name);

        if (length < 0 || (size_t)length >= sizeof(full_path))
        {
            continue;
        }

        struct stat info;

        // S_ISDIR checks directory

        if (stat(full_path, &info) == 0 && S_ISDIR(info.st_mode))
        {
            printf("[DIR]  %s\n", entry->d_name);
        }
        else
        {
            printf("[FILE] %s\n", entry->d_name);
        }
    }

    closedir(directory);

#endif

    return 0;
}

// Creates directory
int fs_create_dir(const char *path)
{
    if (CREATE_DIR(path) == 0)
    {
        return 1;
    }

    return 0;
}

// Removes directory
int fs_remove_dir(const char *path) {
    if (REMOVE_DIR(path) == 0)
    {
        return 1;
    }

    return 0;
}