#include "sprites.h"

void loadSprites(uint32_t spriteData[]) {

    volatile uint32_t *PALETTE = (volatile uint32_t *)(0x500F0000);
    int TOTAL_COLORS = 256 * 4 * 4;

    volatile uint8_t *L_DATA = (volatile uint8_t *)(0x50090000);
    volatile uint8_t *M_DATA = (volatile uint8_t *)(0x500D0000);
    volatile uint8_t *S_DATA = (volatile uint8_t *)(0x500E0000);

    int TOTAL_PIXELS_L = 64 * 64 * 64;
    int TOTAL_PIXELS_M = 32 * 32 * 64;
    int TOTAL_PIXELS_S = 16 * 16 * 256;

    for(int i = 0; i < TOTAL_COLORS; i++) {
        PALETTE[i] = (uint32_t)spriteData[i];
    }

    for(int i = 0; i < TOTAL_PIXELS_L; i++) { 
        L_DATA[i] = (uint8_t)spriteData[TOTAL_COLORS + i];
    }

    for(int i = 0; i < TOTAL_PIXELS_M; i++) { 
        M_DATA[i] = (uint8_t)spriteData[TOTAL_COLORS + TOTAL_PIXELS_L + i];
    }

    for(int i = 0; i < TOTAL_PIXELS_S; i++) { 
        S_DATA[i] = (uint8_t)spriteData[TOTAL_COLORS + TOTAL_PIXELS_L + TOTAL_PIXELS_M + i];
    }
};

int drawSprites(int x, int y, int z, int palette, int index) {
    return 0;
};


// uint32_t MediumControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index){
//     return (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | (((uint32_t)y+32)<<12) | (((uint32_t)x+32)<<2) | (palette & 0x3);
// }