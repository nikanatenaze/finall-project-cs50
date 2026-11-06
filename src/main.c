// global includes
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// project includes
#include <fileinfo.h>
#include <filesystem.h>

// default input size in bytes
#define INPUT_SIZE 128

// displays help
void display_help()
{
    printf("\nAvailable commands:\n");
    printf("  help       - Show available commands\n");
    printf("  pwd        - Show current directory\n");
    printf("  ls         - List directory contents\n");
    printf("  cd <path>  - Change directory\n");
    printf("  back/..    - Go to parent directory\n");
    printf("  exit       - Exit the application\n\n");
}

// displays wellcome title
void display_title()
{
    printf("CS50 final project [Version: 0.0.1B]\n");
    printf("Type 'help' to see available commands.\n");
    printf("(c) Github: https://github.com/nikanatenaze\n");
}

// clears terminal (can be added in another file but here more comfortable)
void clear_terminal(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// main function
int main(void)
{
    // main Data:
    char input[INPUT_SIZE];
    char command[32];
    char argument[480];
    char cwd[4096];

    // gets cwd
    if (fs_getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Unable to get current directory");
        return 1;
    }

    clear_terminal();
    display_title();

    while (true)
    {
        // updates path every tick
        if (fs_getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("Unable to get current directory");
            break;
        }

        // prints main line
        printf("\n");
        printf("[M*] @ %s > ", cwd);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");

            break;
        }

        input[strcspn(input, "\n")] = '\0';

        command[0] = '\0';
        argument[0] = '\0';

        int count = sscanf(input, "%31s %479[^\n]", command, argument);

        // EOF = end of file = -1
        if (count == EOF || count == 0)
        {
            continue;
        }

        if (strcmp(input, "exit") == 0)
        {
            break;
        }
        else if (strcmp(input, "help") == 0)
        {
            display_help();
        }
        // gets shows working dir
        else if (strcmp(input, "pwd") == 0)
        {
            if (fs_getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("Current working directory(cwd): %s\n", cwd);
            }
            else
            {
                printf("Unable to get 'Current working directory'\n");
            }
        }
        // changes current working dir
        else if (strcmp(command, "cd") == 0)
        {
            if (argument[0] == '\0')
            {
                printf("Usage: '... > cd <PATH>'\n");
            }
            else
            {
                if (fs_chdir(argument) != 0)
                {
                    printf("Unable to change directory\n");
                }
            }
        }
        else if (strcmp(command, "ls") == 0)
        {
            fs_list(".");
        }
        else if (strcmp(command, "back") == 0 || strcmp(command, "..") == 0)
        {
            if (fs_chdir("..") != 0)
            {
                perror("Unable to go to parent directory\n");
            }
        }
        else if (strcmp(command, "rmdir") == 0)
        {
            if (argument[0] == '\0')
            {
                printf("Usage: mkdir <directory_name>\n");
            }
            else
            {
                if (fs_remove_dir(argument))
                {
                    continue;
                }
                else
                {
                    printf("Unable to remove directory");
                }
            }
        }
        else if (strcmp(command, "mkdir") == 0)
        {
            if (argument[0] == '\0')
            {
                printf("Usage: mkdir <directory_name>\n");
            }
            else
            {
                if (fs_create_dir(argument))
                {
                    continue;
                }
                else if (errno == EEXIST)
                {
                    printf("Directory already exists.\n");
                }
                else
                {
                    printf("Unable to create directory");
                }
            }
        }
        else if (strcmp(command, "info") == 0)
        {
            if (count < 2)
            {
                printf("Usage: info <path>\n");
            }
            else
            {
                fi_file_info(argument);
            }
        }
        else if (strcmp(command, "opentxt") == 0)
        {
            if (count < 2)
            {
                printf("Usage: open <path>\n");
            }
            else
            {
                fi_open_text(argument);
            }
        }
        else if (strcmp(command, "clear") == 0)
        {
            clear_terminal();
        }
        // continues when no input
        // not needed
        else if (input[0] == '\0')
        {
            continue;
        }
        else
        {
            continue;
            // printf("Unknown command: %s\n", input);
            // printf("Type 'help' to see available commands.\n");
        }
    }

    return 0;
}