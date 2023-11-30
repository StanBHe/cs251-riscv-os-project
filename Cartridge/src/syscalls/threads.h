#ifndef THREADS_H
#define THREADS_H

TThreadContext InitThread();

void SwitchThread(TThreadContext *oldcontext, TThreadContext newcontext);

int exec(const char* path, char* const argv[]);

int kill(int pid);

int wait(int pid);

int get_ppid();

#endif 
