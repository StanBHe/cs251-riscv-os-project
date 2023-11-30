#include <stdint.h>
#include <time.h>
#include "./spriteSheets/spriteData.h"
#include "./syscalls/sprites.h"

int main() {

    loadSprites(SPRITE_DATA);

    // x, y, z, sprite_index, sprite_type, palette, control structure_index

    drawSprites(50, 100, 0, 0, 1, 0, 0);
    drawSprites(100, 100, 0, 1, 1, 0, 1);
    drawSprites(150, 100, 0, 8, 1, 0, 2);
    drawSprites(50, 150, 0, 9, 1, 0, 3);
    drawSprites(100, 150, 0, 16, 1, 0, 4);
    drawSprites(150, 150, 0, 63, 1, 0, 5);

    drawSprites(250, 170, 0, 0, 0, 0, 0);
    drawSprites(290, 170, 0, 1, 0, 0, 1);
    drawSprites(330, 170, 0, 19, 0, 0, 2);

    return 0;
}