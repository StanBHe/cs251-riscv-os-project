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

    // while (1) {
    //     reset = GetReset();
    //     global = GetTicks();
    //     if(global != last_global) {
    //         drawText(2, 4, "Thread 2");
    //         controller_status = GetController();
    //         if(controller_status){
    //             if(controller_status & 0x2){
    //             }
    //             if(controller_status & 0x8){
    //                 drawText(2, 12, "attempt switch from 2");
    //                 SwitchThread(&other_thread, MainThread);
    //             }
    //         }
    //         total++;
    //         if(total > 10) {
    //             drawText(2, 20, "Thread 2 10 ticks");
    //         }

    //         last_global = global;
    //         if(last_reset != reset){
    //             last_reset = reset;
    //         }
    //     }
    // }
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

                last_reset = reset;
            }
        }
    }
    return 0;
}
