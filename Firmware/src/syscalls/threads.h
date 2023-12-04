#ifndef THREADS_H
#define THREADS_H

#include <stdint.h>

typedef uint32_t *TThreadContext;
typedef void (*TThreadEntry)(void *);

int fork();

int exec(const char* path, char* const argv[]);

int kill(int pid);

int wait(int pid);

int get_ppid();

#endif 
