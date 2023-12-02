#include <stdint.h>
// #include <stdlib.h>
#include <time.h>
#include "./syscalls/input.h"
#include "./syscalls/state.h"
#include "./spriteData.h"
#include "./syscalls/graphics.h"
#include "./syscalls/memory.h"

uint32_t SCREEN_WIDTH = 512;
uint32_t SCREEN_HEIGHT = 288;

uint32_t TEXT_WIDTH = 64;
uint32_t TEXT_HEIGHT = 36;

uint32_t BLOCK_SIZE = 8;
uint32_t GRID_OFFSET_X = 207;
uint32_t GRID_OFFSET_Y = 80;

/*
    drawSprites:
    x, y, z, sprite_index, sprite_type, palette, control structure_index
    sprite types: 0 = large, 1 = medium, 2 = small, 3 = background

    Controller Mapping:
          w=02             u=10  i=20
    a=01        d=08  cmd  
          x=04             j=40  k=80

*/

int main() {

    loadSprites(SPRITE_DATA);
    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
    setGraphicsMode(1);
    drawSprite(0, 0, 0, 0, 3, 0, 0);

    int last_reset = GetReset();
    int reset;
    int x_pos = 150;
    int y_pos = 100;
    int last_global = 0;
    int32_t textInd = 0;
    uint32_t global = 0;
    uint32_t controller_status = 0;

    int menuloop = 1;
    int gameloop = 0;
    int scoreloop = 0;

    int** grid = (int**)malloc(sizeof(int*) * 24);
    for(int i = 0; i < 24; i++) {
        grid[i] = (int*)malloc(sizeof(int) * 10);
        for(int k = 0; k < 10; k++) {
            grid[i][k] = 0;
        }
    }

    while (1) {
        reset = GetReset();
        global= GetTicks();

        if(menuloop) {
            if(global != last_global){
                controller_status = GetController();
                if(controller_status){
                    if(controller_status & 0x10){
                        gameloop = 1;
                        menuloop = 0;
                        drawSprite(0, 0, 0, 1, 3, 0, 0);
                        drawSprite(GRID_OFFSET_X, GRID_OFFSET_Y, 1, 48, 1, 0, 0);
                    }
                }
            }
        }

        if(gameloop) {
            if(global != last_global){
                controller_status = GetController();
                if(controller_status){
                    if(controller_status & 0x1){
                    }
                    if(controller_status & 0x8){
                    }
                    if(controller_status & 0x2){
                    }
                    if(controller_status & 0x4){
                    }
                    if(controller_status & 0x10){
                    }
                    if(controller_status & 0x20){
                    }
                    if(controller_status & 0x40){
                    }
                    if(controller_status & 0x80){
                    }
                }
            }
        }
        
        last_global = global;
        if(last_reset != reset){
            last_reset = reset;
            gameloop = 0;
            menuloop = 1;
            drawSprite(512, 288, 0, 0, 3, 0, 0);
        }
    }

    return 0;
}