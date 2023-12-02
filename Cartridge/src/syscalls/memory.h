#include "stdio.h"

#ifndef MEMORY_H
#define MEMORY_H

void* malloc(size_t size);

void* realloc(void* pointer, size_t size);

void free(void* pointer);

#endif 