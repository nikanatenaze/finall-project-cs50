# Nick's Mini File Explorer

A command-line file explorer written in **C** as my CS50 final project. It provides an interactive terminal interface for navigating directories, inspecting files, managing folders, and working with system processes.

The project is designed with separate modules for filesystem operations, file information, and process management, with platform-specific implementations for **Windows and Linux**.

## Features

### Directory navigation
- Display the current working directory with `pwd`
- List files and folders with `ls`
- Move to another directory with `cd <path>`
- Return to the parent directory with `back` or `..`

### File and directory management
- Create a directory with `mkdir <name>`
- Remove an empty directory with `rmdir <name>`
- Display file or directory information with `info <path>`
- Read a text file in the terminal with `opentxt <path>`

### Process management
- Launch an application with `run <program>`
- List running processes with `processes`
- Display the current process ID with `cpid`
- Inspect a process with `pidi <PID>`
- Terminate a process with `end <PID>`

### Terminal
- Display the command list with `help`
- Clear the terminal with `clear`
- Exit the application with `exit`

## Project structure

```text
finall-project-cs50/
├── include/
│   ├── fileinfo.h
│   ├── filesystem.h
│   └── process.h
├── src/
│   ├── fileinfo.c
│   ├── filesystem.c
│   ├── main.c
│   └── process.c
└── README.md
```

- **main.c** — command-line interface and command handling
- **filesystem.c** — directory navigation and directory operations
- **fileinfo.c** — file metadata and text-file reading
- **process.c** — process launching, listing, inspection, and termination

## Requirements

- A C compiler, such as GCC
- Windows or Linux

Some functionality uses operating-system-specific APIs. The source code selects the relevant implementation at compile time using `_WIN32`.

## Build

Compile the source files from the project root with GCC:

**Linux**
```bash
gcc -std=c11 -Wall -Wextra -Iinclude src/main.c src/filesystem.c src/fileinfo.c src/process.c -o mini-explorer
```

**Windows (MinGW-w64)**
```bash
gcc -std=c11 -Wall -Wextra -Iinclude src/main.c src/filesystem.c src/fileinfo.c src/process.c -o mini-explorer.exe
```

Then run the executable:

**Linux**
```bash
./mini-explorer
```

**Windows**
```powershell
.\mini-explorer.exe
```

You can optionally provide a starting directory:

```bash
./mini-explorer /path/to/directory
```

On Windows, pass a Windows directory path instead.

## Usage

Start the program and enter `help` to see all available commands.

Example session:

```text
[M*] @ /home/user > pwd
[M*] @ /home/user > ls
[M*] @ /home/user > mkdir projects
[M*] @ /home/user > cd projects
[M*] @ /home/user/projects > back
[M*] @ /home/user > exit
```

## About the project

This project was developed as the final project for **CS50x**. Its goal is to apply C programming concepts to a practical command-line application, including modular code organization, file and directory operations, process management, and interaction with operating-system APIs.

---

**Author:** [Nikoloz Natenadze](https://github.com/nikanatenaze)  
**Repository:** [finall-project-cs50](https://github.com/nikanatenaze/finall-project-cs50)
