#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "./threads.h"


TThreadContext createThread(TThreadEntry entry, void *param) {
    uint32_t *OtherThreadStack = (uint32_t*) malloc(128 * sizeof(uint32_t));

    return sys_createThread(OtherThreadStack, entry, param);
}


