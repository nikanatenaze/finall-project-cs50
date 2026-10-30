// global includes
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// project includes
#include <filesystem.h>

#define INPUT_SIZE 128

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

void display_title()
{
    printf("\n================================\n");
    printf("  NICK'S MINI FILE EXPLORER\n");
    printf("================================\n");
    printf("Type 'help' to see available commands.\n\n");
}

int main(void)
{
    // main Data:
    char input[INPUT_SIZE];
    char command[32];
    char argument[480];
    char cwd[4096];

    // gets cwd for enterance title
    if (fs_getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Unable to get current directory");
        return 1;
    }

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
                printf("%s\n", cwd);
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
            if (cwd[0] == '\n')
            {
                printf("Unable to display without arguments\n");
            }
            else
            {
                fs_list(".");
            }
        }
        else if (strcmp(command, "back") == 0 || strcmp(command, "..") == 0)
        {
            if (fs_chdir("..") != 0)
            {
                perror("Unable to go to parent directory");
            }
        }
        

        // continues when no input
        else if (input[0] == '\0')
        {
            continue;
        }
        else
        {
            printf("Unknown command: %s\n", input);
            printf("Type 'help' to see available commands.\n");
        }
    }

    return 0;
}