#include "threads.h"
#include <stdint.h>

typedef uint32_t *TThreadContext;

TThreadContext InitThread() {
    return 0;
};

void SwitchThread(TThreadContext *oldcontext, TThreadContext newcontext) {};

int exec(const char* path, char* const argv[]) {
    return 0;
};

int kill(int pid) {
    return 0;
};

int wait(int pid) {
    return 0;
};

int get_ppid() {
    return 0;
};

