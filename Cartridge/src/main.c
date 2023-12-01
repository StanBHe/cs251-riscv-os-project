#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "./syscalls/input.h"
#include "./syscalls/state.h"
#include "./spriteSheets/spriteData.h"
#include "./syscalls/graphics.h"

uint32_t SCREEN_WIDTH = 512;
uint32_t SCREEN_HEIGHT = 288;

uint32_t TEXT_WIDTH = 64;
uint32_t TEXT_HEIGHT = 36;

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

    drawSprite(512, 288, 0, 0, 3, 0, 0);

    drawSprite(50, 100, 1, 0, 1, 0, 0);
    drawSprite(100, 100, 1, 1, 1, 0, 1);
    drawSprite(150, 100, 1, 8, 1, 0, 2);
    drawSprite(50, 150, 1, 9, 1, 0, 3);
    drawSprite(100, 150, 1, 16, 1, 0, 4);
    drawSprite(150, 150, 1, 63, 1, 0, 5);

    drawSprite(415, 270, 1, 0, 0, 0, 0);
    drawSprite(450, 270, 1, 1, 0, 0, 1);
    drawSprite(500, 270, 1, 19, 0, 0, 2);

    drawSprite(40, 220, 1, 16*0, 2, 0, 0);
    drawSprite(60, 220, 1, 16*1, 2, 0, 1);
    drawSprite(80, 220, 1, 16*2, 2, 0, 2);
    drawSprite(100, 220, 1, 16*3, 2, 0, 3);
    drawSprite(120, 220, 1, 16*4, 2, 0, 4);
    drawSprite(140, 220, 1, 16*5, 2, 0, 5);
    drawSprite(160, 220, 1, 16*6, 2, 0, 6);

    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
    setGraphicsMode(1);

    int last_reset = GetReset();
    int reset;
    int x_pos = 150;
    int y_pos = 100;
    int last_global = 0;
    int32_t textInd = 0;
    uint32_t global = 0;
    uint32_t controller_status = 0;

    while (1) {
        reset = GetReset();
        global= GetTicks();
        if(global != last_global){
            controller_status = GetController();
            if(controller_status){
                if(controller_status & 0x1){
                    if(x_pos > 0){
                        x_pos--;
                    }
                }
                if(controller_status & 0x8){
                    if(x_pos < SCREEN_WIDTH) {
                        x_pos++;
                    }
                }
                if(controller_status & 0x2){
                    if(y_pos > 0){
                        y_pos--;
                    }
                }
                if(controller_status & 0x4){
                    if(y_pos < SCREEN_HEIGHT){
                        y_pos++;
                    }
                }
                if(controller_status & 0x10){
                    setGraphicsMode(0);
                }
                if(controller_status & 0x20){
                    setGraphicsMode(1);
                }
                if(controller_status & 0x40){
                    drawSprite(512, 288, 0, 0, 3, 0, 0);
                    drawText(((x_pos * TEXT_WIDTH) / SCREEN_WIDTH), (((y_pos * TEXT_HEIGHT) / SCREEN_HEIGHT) + 1), "Bottom Text");
                }
                if(controller_status & 0x80){
                    clearSprite(3, 0);
                    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
                }

                drawSprite(x_pos, y_pos, 1, 8, 1, 0, 2);
                clearText(textInd);
                textInd = drawText(((x_pos * TEXT_WIDTH) / SCREEN_WIDTH), ((y_pos * TEXT_HEIGHT) / SCREEN_HEIGHT), "Top Text");

            }
            last_global = global;
            if(last_reset != reset){
                x_pos = 0;
                y_pos = 0;
                last_reset = reset;
            }
        }
    }

    return 0;
}