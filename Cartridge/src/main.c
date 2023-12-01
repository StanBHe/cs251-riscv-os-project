#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "./syscalls/input.h"
#include "./syscalls/state.h"
#include "./spriteSheets/spriteData.h"
#include "./syscalls/sprites.h"

int main() {

    loadSprites(SPRITE_DATA);

    // x, y, z, sprite_index, sprite_type, palette, control structure_index
    // sprite types: 0 = large, 1 = medium, 2 = small, 3 = background

    drawSprites(512, 288, 0, 0, 3, 0, 0);

    drawSprites(50, 100, 1, 0, 1, 0, 0);
    drawSprites(100, 100, 1, 1, 1, 0, 1);
    drawSprites(150, 100, 1, 8, 1, 0, 2);
    drawSprites(50, 150, 1, 9, 1, 0, 3);
    drawSprites(100, 150, 1, 16, 1, 0, 4);
    drawSprites(150, 150, 1, 63, 1, 0, 5);

    drawSprites(415, 270, 1, 0, 0, 0, 0);
    drawSprites(450, 270, 1, 1, 0, 0, 1);
    drawSprites(500, 270, 1, 19, 0, 0, 2);

    drawSprites(40, 220, 1, 16*0, 2, 0, 0);
    drawSprites(60, 220, 1, 16*1, 2, 0, 1);
    drawSprites(80, 220, 1, 16*2, 2, 0, 2);
    drawSprites(100, 220, 1, 16*3, 2, 0, 3);
    drawSprites(120, 220, 1, 16*4, 2, 0, 4);
    drawSprites(140, 220, 1, 16*5, 2, 0, 5);
    drawSprites(160, 220, 1, 16*6, 2, 0, 6);

    return 0;
}