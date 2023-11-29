#include <stdint.h>
#include <time.h>
#include "./spriteSheets/spriteData.h"

volatile uint32_t *MEDIUM_PALETTE = (volatile uint32_t *)(0x500F2000);
volatile uint32_t *MEDIUM_CONTROL = (volatile uint32_t *)(0x500F5F00);
volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);

// void loadSprites(int spriteData[]);

uint32_t MediumControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index);

int main() {

    loadSprites(SPRITE_DATA);

    // MEDIUM_PALETTE[0] = 0xFFFFFFFF; // A R G B
    // MEDIUM_PALETTE[0] = 0xFFFF0000; 
    // MEDIUM_PALETTE[1] = 0xFFFFFF00; 
    // MEDIUM_PALETTE[2] = 0xFF00FF00;
    // MEDIUM_PALETTE[256] = 0xFF00FF00;

    MEDIUM_CONTROL[0] = MediumControl(0, 50, 100, 0, 0);
    MEDIUM_CONTROL[1] = MediumControl(0, 100, 100, 0, 1);
    MEDIUM_CONTROL[2] = MediumControl(0, 150, 100, 0, 8);
    MEDIUM_CONTROL[3] = MediumControl(0, 50, 150, 1, 9);
    MEDIUM_CONTROL[4] = MediumControl(0, 100, 150, 1, 16);
    MEDIUM_CONTROL[5] = MediumControl(0, 150, 150, 1, 63);


    *MODE_REGISTER = 1;

    return 0;
}

uint32_t MediumControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index){
    return (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | (((uint32_t)y+32)<<12) | (((uint32_t)x+32)<<2) | (palette & 0x3);
}