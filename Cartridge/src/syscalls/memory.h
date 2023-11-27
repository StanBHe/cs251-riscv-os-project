#ifndef MEMORY_H
#define MEMORY_H

void* malloc(size_t size);

void* realloc(void* pointer);

int free(void* pointer);

#endif 