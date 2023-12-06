#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "./syscalls/events.h"
#include "./syscalls/graphics.h"
#include "./syscalls/input.h"
#include "./syscalls/state.h"
#include "./syscalls/threads.h"
#include "./syscalls/time.h"
#include "./usefulValues.h"
#include "./spriteData.h"

/*  
    Sample Cartridge with two threads to switch between, 
    some display of text and sprites, 
    and all syscall .H files included 

    Images loaded are the ones in made in Sprite Kit (Sample) 
    after running the notebook
*/

TThreadContext other_thread;
TThreadContext MainThread;

volatile int last_global = 0;
volatile int global = 0;
volatile int controller_status = 0;
volatile int last_reset;
volatile int reset;

void otherThreadFunc();

int main() {

    // do this to load sprites, clear text from os startup, and set last reset
    loadSprites(SPRITE_DATA);
    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
    last_reset = getReset();

    // draw some sprites
    drawSprite(0, 0, 0, 0, BACKGROUND_T, 0, 0);
    drawSprite(400, 200, 1, 17, MEDIUM_T, 0, 0);
    // make sure to change control structures with last param 
    drawSprite(430, 200, 1, 63, MEDIUM_T, 0, 1);
    drawSprite(460, 200, 1, 62, MEDIUM_T, 0, 2); 

    // draw some text
    drawText(2, 2, "In Cartridge");
    drawText(2, 6, "Press u to switch to graphics mode");
    drawText(2, 8, "Press i to switch to text mode");
    drawText(2, 12, "Press j to switch to thread 1");
    drawText(2, 14, "Press k to switch to thread 2");

    other_thread = createThread(otherThreadFunc, NULL);

    while (1) {
        reset = getReset();
        global = getTicks();
        if(global != last_global){
            drawText(2, TEXT_HEIGHT - 2, "In Thread 1");
            controller_status = getController();
            if(controller_status){
                if(controller_status & K_KEY){
                    switchThread(&MainThread, other_thread);
                }
                if(controller_status & U_KEY) {
                    setGraphicsMode(GRAPHICS_MODE);
                }
                if(controller_status & I_KEY) {
                    setGraphicsMode(TEXT_MODE);
                }
            }

            last_global = global;
            if(last_reset != reset){
                drawText(TEXT_WIDTH - 20, TEXT_HEIGHT - 2, "CMD Button Pressed");
                last_reset = reset;
            }
        }
    }
    return 0;
}

void otherThreadFunc() {
    while (1) {
        reset = getReset();
        global = getTicks();
        if(global != last_global) {
            drawText(2, TEXT_HEIGHT - 2, "In Thread 2");
            controller_status = getController();
            if(controller_status){
                if(controller_status & J_KEY){
                    switchThread(&other_thread, MainThread);
                }
                if(controller_status & U_KEY) {
                    setGraphicsMode(GRAPHICS_MODE);
                }
                if(controller_status & I_KEY) {
                    setGraphicsMode(TEXT_MODE);
                }
            }

            last_global = global;
            if(last_reset != reset){
                drawText(TEXT_WIDTH - 20, TEXT_HEIGHT - 2, "CMD Button Pressed");
                last_reset = reset;
            }
        }
    }
}