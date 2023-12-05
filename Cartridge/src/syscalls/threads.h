#ifndef THREADS_H
#define THREADS_H

// TThreadContext InitThread();
typedef void (*TThreadEntry)(void *);
typedef uint32_t *TThreadContext;

TThreadContext createThread(TThreadEntry entry, void *param);
TThreadContext sys_createThread(uint32_t *OtherThreadStack, TThreadEntry entry, void *param);

void SwitchThread(TThreadContext *oldcontext, TThreadContext newcontext);

int exec(const char* path, char* const argv[]);

int kill(int pid);

int wait(int pid);

int get_ppid();

#endif 
