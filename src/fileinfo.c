#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <fileinfo.h>

#ifdef _WIN32
#define stat _stat
#endif

int fi_file_info(const char *path)
{
    struct stat info;

    if (stat(path, &info) != 0)
    {
        perror("Unable to fetch data");
        return -1;
    }

    printf("\n----- FILE INFORMATION -----\n");
    printf("Path: %s\n", path);

// windows friendly
#ifdef _WIN32
    if (info.st_mode & _S_IFDIR)
    {
        printf("Type: Directory\n");
    }
    else if (info.st_mode & _S_IFREG)
    {
        printf("Type: Regular file\n");
    }
    else
    {
        printf("Type: Other\n");
    }

// linux friendly
#else
    if (S_ISDIR(info.st_mode))
    {
        printf("Type: Directory\n");
    }
    else if (S_ISREG(info.st_mode))
    {
        printf("Type: Regular file\n");
    }
    else if (S_ISLNK(info.st_mode))
    {
        printf("Type: Symbolic link\n");
    }
    else
    {
        printf("Type: Other\n");
    }
#endif

    // prints file size in bytes
    // (long long) is used to very very very large integers
    // %lld is variable for long long
    printf("Size: %lld bytes\n", (long long)info.st_size);

    printf("----------------------------\n\n");

    return 0;
}

int fi_open_text(const char *path)
{
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Unable to open file\n");
        return -1;
    }

    char *buffer = malloc(sizeof(char) * 1024);
    printf("\n-FILE CONTENT------------\n\n");

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        fputs(buffer, stdout);
    }

    if (ferror(file))
    {
        perror("Error while reading file");
        fclose(file);
        return -1;
    }

    printf("\n\n-------------------------\n\n");

    fclose(file);
    free(buffer);

    return 0;
}