#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "./syscalls/input.h"
#include "./syscalls/threads.h"
#include "./syscalls/graphics.h"

TThreadContext other_thread;
TThreadContext MainThread;

void otherThreadFunc() {
    int last_global = 42;
    uint32_t global = 42;
    uint32_t controller_status = 0;

    int last_reset = GetReset();
    int reset;
    int total = 0;

    drawText(2, 6, "Thread 2");

    while (1) {
        reset = GetReset();
        global = GetTicks();
        if(global != last_global) {
            drawText(2, 4, "Thread 2");
            controller_status = GetController();
            if(controller_status){
                if(controller_status & 0x2){
                }
                if(controller_status & 0x8){
                    drawText(2, 12, "attempt switch from 2");
                    SwitchThread(other_thread, &MainThread);
                }
            }
            total++;
            if(total > 10) {
                drawText(2, 20, "Thread 2 10 ticks");
            }

            last_global = global;
            if(last_reset != reset){
                last_reset = reset;
            }
        }
    }
    while(1){
        SwitchThread(&other_thread, MainThread);
    }
    //return a + b;
}

int main() {
    int last_global = 42;
    uint32_t global = 42;
    uint32_t controller_status = 0;

    int last_reset = GetReset();
    int reset;

    drawText(2, 2, "In Cartridge");

    other_thread = createThread(otherThreadFunc, NULL);
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

    //uint32_t *OtherThreadStack = (uint32_t*) malloc(128 * sizeof(uint32_t));
    //uint32_t OtherThreadStack[128];
    //other_thread = sys_createThread(OtherThreadStack, blank_func, NULL);

    other_thread = createThread(/*(void*)*/blank_func, NULL);

    //MainThread = (TThreadContext) malloc(128 * sizeof(uint32_t));

    while (1) {
        drawText(2, 4, "Thread 1");
        reset = GetReset();
        global= GetTicks();
        if(global != last_global){
            controller_status = GetController();
            if(controller_status){
                if(controller_status & 0x1){
                    drawText(2, 10, "going to thread 2");
                    SwitchThread(&MainThread, other_thread);
                }
            }
            last_global = global;
            if(last_reset != reset){
                // createThread((void*)blank_func, NULL);
                SwitchThread(&MainThread, other_thread);

                x_pos = 0;
                last_reset = reset;
            }
        }
    }
    return 0;
}
