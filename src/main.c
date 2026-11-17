// global includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

// project includes
#include <process.h>
#include <fileinfo.h>
#include <filesystem.h>

// default input size in bytes
#define INPUT_SIZE 128

// displays help
// displays help
void display_help(void)
{
    printf("\n========== AVAILABLE COMMANDS ==========\n\n");

    printf(" General:\n");
    printf("  help                  Show available commands\n");
    printf("  clear                 Clear the terminal\n");
    printf("  exit                  Exit the application\n\n");

    printf(" Navigation:\n");
    printf("  pwd                   Show current working directory\n");
    printf("  ls                    List directory contents\n");
    printf("  cd <path>             Change current directory\n");
    printf("  back / ..             Go to parent directory\n\n");

    printf(" File & Directory Management:\n");
    printf("  mkdir <name>          Create a directory\n");
    printf("  rmdir <name>          Remove an empty directory\n");
    printf("  info <path>           Display file information\n");
    printf("  opentxt <path>        Open and read a text file\n\n");

    printf(" Process Management:\n");
    printf("  run <program>         Launch an application\n");
    printf("  processes             List running processes\n");
    printf("  cpid                  Show current process ID\n");
    printf("  pidi <PID>            Display process information\n");
    printf("  end <PID>             Terminate a process\n");

    printf("\n========================================\n");
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
int main(int argc, char *argv[])
{
    // main Data:
    char input[INPUT_SIZE];
    char command[32];
    char argument[480];
    char cwd[4096];

    // gets cwd
    if (argc > 1)
    {
        if (!is_valid_directory(argv[1]))
        {
            fprintf(stderr, "Error: Invalid directory: %s\n", argv[1]);
            return 1;
        }

        if (fs_chdir(argv[1]) != 0)
        {
            perror("Unable to change starting directory");
            return 1;
        }
    }
    else if (fs_getcwd(cwd, sizeof(cwd)) == NULL)
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
        else if (strcmp(command, "run") == 0)
        {
            if (count < 2)
            {
                printf("Usage: run <application>\n");
            }
            else
            {
                ProcessID pid;

                if (process_launch(argument, &pid) == 0)
                {
                    printf("Application started successfully!\n");
                    printf("PID: %lu\n", (unsigned long)pid);
                }
            }
        }
        else if (strcmp(command, "processes") == 0)
        {
            show_processes();
        }
        else if (strcmp(command, "end") == 0)
        {
            if (argument[0] == '\0')
            {
                printf("Usage: end <PID>\n");
            }
            else
            {
                char *endptr;
                long pid = strtol(argument, &endptr, 10);

                // Validate that the entire argument is a positive number
                if (argument == endptr || *endptr != '\0' || pid <= 0)
                {
                    printf("Invalid PID: '%s'\n", argument);
                }
                else
                {
                    endprocess((int)pid);
                }
            }
        }
        else if (strcmp(command, "cpid") == 0)
        {
            int cpid = get_cpid();
            printf("PID of current process: %i\n", cpid);
        }
        else if (strcmp(command, "pidi") == 0)
        {
            if (argument[0] == '\0')
            {
                printf("Usage: pidi <PID>\n");
            }
            else
            {
                char *endptr;
                long pid = strtol(argument, &endptr, 10);

                // Validate that the entire argument is a positive number
                if (argument == endptr || *endptr != '\0' || pid <= 0)
                {
                    printf("Invalid PID: '%s'\n", argument);
                }
                else
                {
                    process_info((int)pid);
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