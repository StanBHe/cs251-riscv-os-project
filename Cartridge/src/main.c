#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "./syscalls/input.h"
#include "./syscalls/threads.h"


volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
volatile uint8_t *MEDIUM_DATA = (volatile uint8_t *)(0x500D0000);
volatile uint32_t *MEDIUM_PALETTE = (volatile uint32_t *)(0x500F2000);
volatile uint32_t *MEDIUM_CONTROL = (volatile uint32_t *)(0x500F5F00);
volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);

uint32_t MediumControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index);

TThreadContext other_thread;
TThreadContext MainThread;

void blank_func(){
    int a = 1;
    int b = 2;

    SwitchThread(&MainThread, other_thread);
    //return a + b;
}

int main() {
    int last_global = 42;
    int x_pos = 12;

    uint32_t global = 42;
    uint32_t controller_status = 0;

    VIDEO_MEMORY[0] = 'H';
    VIDEO_MEMORY[1] = 'e';
    VIDEO_MEMORY[2] = 'l';
    VIDEO_MEMORY[3] = 'l';
    VIDEO_MEMORY[4] = 'o';
    VIDEO_MEMORY[5] = ' ';
    VIDEO_MEMORY[6] = 'W';
    VIDEO_MEMORY[7] = 'o';
    VIDEO_MEMORY[8] = 'r';
    VIDEO_MEMORY[9] = 'l';
    VIDEO_MEMORY[10] = 'd';
    VIDEO_MEMORY[11] = '!';
    VIDEO_MEMORY[12] = 'X';

    // Fill out sprite data
    for(int y = 0; y < 32; y++){
        for(int x = 0; x < 32; x++){    
            MEDIUM_DATA[y*32+x] = 1;
        }
    }
    MEDIUM_PALETTE[1] = 0xFFFF0000; // A R G B
    MEDIUM_PALETTE[257] = 0xFF00FF00;
    //MEDIUM_PALETTE[513] = 0xFF0000FF;
    MEDIUM_CONTROL[0] = MediumControl(0, 0, 0, 0, 0);
    *MODE_REGISTER = 1;
    int last_reset = GetReset();
    int reset;

    // uint32_t *OtherThreadStack = (uint32_t*) malloc(128 * sizeof(uint32_t));
    // other_thread = sys_createThread(OtherThreadStack, blank_func, NULL);

    other_thread = createThread(/*(void*)*/blank_func, NULL);


    while (1) {
        reset = GetReset();
        global= GetTicks();
        if(global != last_global){
            controller_status = GetController();
            if(controller_status){
                VIDEO_MEMORY[x_pos] = ' ';
                if(controller_status & 0x1){
                    if(x_pos & 0x3F){
                        x_pos--;
                    }
                }
                if(controller_status & 0x2){
                    if(x_pos >= 0x40){
                        x_pos -= 0x40;
                    }
                }
                if(controller_status & 0x4){
                    if(x_pos < 0x8C0){
                        x_pos += 0x40;
                    }
                }
                if(controller_status & 0x8){
                    if((x_pos & 0x3F) != 0x3F){
                        x_pos++;
                    }
                }
                VIDEO_MEMORY[x_pos] = 'X';
            }
            last_global = global;
            if(last_reset != reset){
                // createThread((void*)blank_func, NULL);
                SwitchThread(&other_thread, MainThread);

                x_pos = 0;
                last_reset = reset;
            }
            MEDIUM_CONTROL[0] = MediumControl((global / 10) % 2, (x_pos & 0x3F)<<3, (x_pos>>6)<<3, 0, 0);
        }
    }
    return 0;
}

uint32_t MediumControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index){
    return (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | (((uint32_t)y+32)<<12) | (((uint32_t)x+32)<<2) | (palette & 0x3);
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