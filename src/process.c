#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/process.h"

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#endif

// launches new process
int process_launch(const char *program, ProcessID *pid)
{
    if (program == NULL || program[0] == '\0' || pid == NULL)
    {
        fprintf(stderr, "Invalid program or PID pointer.\n");
        return -1;
    }

// optimised for windows os
#ifdef _WIN32

    STARTUPINFOA startup;
    PROCESS_INFORMATION process;

    ZeroMemory(&startup, sizeof(startup));
    ZeroMemory(&process, sizeof(process));

    startup.cb = sizeof(startup);

    // Create a mutable command line
    size_t length = strlen(program) + 3;

    char *command_line = malloc(length);

    if (command_line == NULL)
    {
        perror("Memory allocation failed");
        return -1;
    }

    snprintf(command_line, length, "\"%s\"", program);

    BOOL success = CreateProcessA(
        NULL,
        command_line,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_PROCESS_GROUP,
        NULL,
        NULL,
        &startup,
        &process);

    free(command_line);

    if (!success)
    {
        fprintf(stderr, "Unable to launch application. Error: %lu\n",
                GetLastError());
        return -1;
    }

    *pid = process.dwProcessId;

    CloseHandle(process.hThread);

    // Wait until the launched application exits.
    WaitForSingleObject(process.hProcess, INFINITE);

    CloseHandle(process.hProcess);

// another os
#else

    pid_t child = fork();

    if (child < 0)
    {
        perror("Unable to create process");
        return -1;
    }

    if (child == 0)
    {
        char *const arguments[] = {
            (char *)program,
            NULL};

        execvp(program, arguments);

        // Reached only if execvp fails
        perror("Unable to execute application");
        _exit(127);
    }

    *pid = (ProcessID)child;

    // Wait until the child process exits.
    waitpid(child, NULL, 0);

#endif

    return 0;
}

#ifndef _WIN32
static int is_numeric_pid(const char *str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)str[i]))
        {
            return 0;
        }
    }
    return 1;
}
#endif

// shows list of processes
int show_processes(void)
{
#ifdef _WIN32
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return -1;
    }

    do
    {
        printf("PID: %lu\tName: %s\n", pe32.th32ProcessID, pe32.szExeFile);
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
#else
    DIR *proc = opendir("/proc");
    if (proc == NULL)
    {
        perror("Unable to open /proc");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL)
    {
        if (is_numeric_pid(entry->d_name))
        {
            char path[256];
            snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);

            FILE *cmd = fopen(path, "r");
            if (cmd != NULL)
            {
                char name[256];
                if (fgets(name, sizeof(name), cmd) != NULL)
                {
                    name[strcspn(name, "\n")] = '\0';
                    printf("PID: %s\tName: %s\n", entry->d_name, name);
                }
                fclose(cmd);
            }
        }
    }

    closedir(proc);
#endif

    return 0;
}

// endprocess

int endprocess(int PID)
{
    if (PID <= 0)
    {
        printf("Usage: stop <PID>\n");
        return 0;
    }

    bool result = false;

#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, (DWORD)PID);
    if (hProcess == NULL)
    {
        printf("Failed to access process %i (check permissions or PID).\n", PID);
        return 0;
    }

    result = (TerminateProcess(hProcess, 1) != 0);
    CloseHandle(hProcess);
#else
    result = (kill((pid_t)PID, SIGTERM) == 0);
#endif

    if (result)
    {
        printf("Successfully ended process %i\n", PID);
    }
    else
    {
        printf("Failed to end process %i\n", PID);
    }

    return result ? 1 : 0;
}

// get current PID
int get_cpid()
{
#if _WIN32
    int cpid =(int)GetCurrentProcessId();
    return cpid;
#else
    int cpid =(int)getpid()
    return cpid;
#endif
}


// displays information about process
int process_info(int PID)
{
    if (PID <= 0)
    {
        printf("Usage: process_info <PID>\n");
        return -1;
    }

#ifdef _WIN32

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE)
    {
        perror("Unable to create process snapshot");
        return -1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    BOOL found = FALSE;

    if (Process32First(snapshot, &pe32))
    {
        do
        {
            if (pe32.th32ProcessID == (DWORD)PID)
            {
                found = TRUE;

                printf("\n========== PROCESS INFORMATION ==========\n");
                printf("PID:             %lu\n", pe32.th32ProcessID);
                printf("Name:            %s\n", pe32.szExeFile);
                printf("Parent PID:      %lu\n", pe32.th32ParentProcessID);
                printf("Threads:         %lu\n", pe32.cntThreads);
                printf("Base Priority:   %ld\n", pe32.pcPriClassBase);

                HANDLE hProcess = OpenProcess(
                    PROCESS_QUERY_LIMITED_INFORMATION,
                    FALSE,
                    (DWORD)PID
                );

                if (hProcess != NULL)
                {
                    char path[MAX_PATH];
                    DWORD size = MAX_PATH;

                    if (QueryFullProcessImageNameA(
                        hProcess, 0, path, &size))
                    {
                        printf("Executable:      %s\n", path);
                    }

                    DWORD exitCode;

                    if (GetExitCodeProcess(hProcess, &exitCode))
                    {
                        printf("State:           %s\n",
                               exitCode == STILL_ACTIVE
                                   ? "Running"
                                   : "Terminated");
                    }

                    CloseHandle(hProcess);
                }
                else
                {
                    printf("Executable:      Access denied\n");
                }

                printf("=========================================\n");
                break;
            }

        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);

    if (!found)
    {
        printf("Process %d not found.\n", PID);
        return -1;
    }

#else

    char path[256];
    char executable[4096] = "Unknown";

    snprintf(path, sizeof(path), "/proc/%d/status", PID);

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Process %d not found or access denied.\n", PID);
        return -1;
    }

    char line[256];
    char name[256] = "Unknown";
    char state = '?';

    int parent_pid = -1;
    int threads = -1;
    unsigned long memory = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "Name:", 5) == 0)
        {
            sscanf(line + 5, "%255[^\n]", name);
        }
        else if (strncmp(line, "State:", 6) == 0)
        {
            sscanf(line + 6, " %c", &state);
        }
        else if (strncmp(line, "PPid:", 5) == 0)
        {
            sscanf(line + 5, "%d", &parent_pid);
        }
        else if (strncmp(line, "VmRSS:", 6) == 0)
        {
            sscanf(line + 6, "%lu", &memory);
        }
        else if (strncmp(line, "Threads:", 8) == 0)
        {
            sscanf(line + 8, "%d", &threads);
        }
    }

    fclose(file);

    // Get executable path
    snprintf(path, sizeof(path), "/proc/%d/exe", PID);

    ssize_t length = readlink(path, executable,
                              sizeof(executable) - 1);

    if (length != -1)
    {
        executable[length] = '\0';
    }

    const char *state_name;

    switch (state)
    {
        case 'R': state_name = "Running"; break;
        case 'S': state_name = "Sleeping"; break;
        case 'D': state_name = "Uninterruptible sleep"; break;
        case 'T': state_name = "Stopped"; break;
        case 'Z': state_name = "Zombie"; break;
        case 'I': state_name = "Idle"; break;
        default:  state_name = "Unknown"; break;
    }

    printf("\n========== PROCESS INFORMATION ==========\n");
    printf("PID:             %d\n", PID);
    printf("Name:            %s\n", name);
    printf("Executable:      %s\n", executable);
    printf("Parent PID:      %d\n", parent_pid);
    printf("State:           %s\n", state_name);
    printf("Threads:         %d\n", threads);
    printf("Memory:          %lu kB\n", memory);
    printf("=========================================\n");

#endif

    return 0;
}
