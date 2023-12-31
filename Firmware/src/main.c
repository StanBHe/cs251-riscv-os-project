#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "./syscalls/graphics.h"
#include "./syscalls/threads.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile int reset = 0;

volatile uint32_t *CartridgeStatus = (volatile uint32_t *)(0x4000001C);
typedef void (*FunctionPtr)(void);

int main() {

    drawText(1, 1, "OS Started");
    while (1){
        if(*CartridgeStatus & 0x1){
            FunctionPtr Fun = (FunctionPtr)((*CartridgeStatus) & 0xFFFFFFFC);
            drawText(1, 1, "Loading Cartridge...");
            Fun();
        }
    }
    return 0;
}

extern char _heap_base[];
extern char _stack[];

char *_sbrk(int numbytes){
  static char *heap_ptr = NULL;
  char *base;

  if (heap_ptr == NULL) {
    heap_ptr = (char *)&_heap_base;
  }

  if((heap_ptr + numbytes) <=_stack) {
    base = heap_ptr;
    heap_ptr += numbytes;
    return (base);
  }
  else {
    //errno = ENOMEM;
    return NULL;
  }
}