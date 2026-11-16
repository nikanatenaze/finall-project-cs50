#ifndef PROCESS_H
#define PROCESS_H

#ifdef _WIN32
    typedef unsigned long ProcessID;
#else
    typedef int ProcessID;
#endif

int process_launch(const char *program, ProcessID *pid);
int show_processes();
int endprocess(int PID);
int get_cpid();
int process_info(int PID);

#endif
