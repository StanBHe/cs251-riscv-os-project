#include <stdint.h>
#include <time.h>
#include "./spriteSheets/spriteData.h"

volatile uint32_t *LARGE_CONTROL = (volatile uint32_t *)(0x500F5B00);
volatile uint32_t *MEDIUM_CONTROL = (volatile uint32_t *)(0x500F5F00);
volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);

// void loadSprites(int spriteData[]);

uint32_t SpriteControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index);

int main() {

    loadSprites(SPRITE_DATA);

    MEDIUM_CONTROL[0] = SpriteControl(0, 50, 100, 0, 0);
    MEDIUM_CONTROL[1] = SpriteControl(0, 100, 100, 0, 1);
    MEDIUM_CONTROL[2] = SpriteControl(0, 150, 100, 0, 8);
    MEDIUM_CONTROL[3] = SpriteControl(0, 50, 150, 1, 9);
    MEDIUM_CONTROL[4] = SpriteControl(0, 100, 150, 1, 16);
    MEDIUM_CONTROL[5] = SpriteControl(0, 150, 150, 1, 63);
    // MEDIUM_CONTROL[0] = SpriteControl(0, 200, 150, 0, 62);

    LARGE_CONTROL[0] = SpriteControl(0, 250, 170, 2, 0);
    LARGE_CONTROL[1] = SpriteControl(0, 290, 170, 3, 1);
    LARGE_CONTROL[2] = SpriteControl(0, 330, 170, 4, 19);

    *MODE_REGISTER = 1;

    return 0;
}

uint32_t SpriteControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index){
    return (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | (((uint32_t)y+32)<<12) | (((uint32_t)x+32)<<2) | (palette & 0x3);
}
