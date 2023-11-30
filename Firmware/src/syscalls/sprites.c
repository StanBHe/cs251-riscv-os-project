#include "sprites.h"

void loadSprites(uint32_t spriteData[]) {

    volatile uint32_t *PALETTE = (volatile uint32_t *)(0x500F0000);
    volatile uint8_t *L_DATA = (volatile uint8_t *)(0x50090000);
    volatile uint8_t *M_DATA = (volatile uint8_t *)(0x500D0000);
    volatile uint8_t *S_DATA = (volatile uint8_t *)(0x500E0000);
    volatile uint8_t *B_DATA = (volatile uint8_t *)(0x50000000);

    int TOTAL_COLORS = 256 * 4 * 4;
    int TOTAL_PIXELS_L = 64 * 64 * 64;
    int TOTAL_PIXELS_M = 32 * 32 * 64;
    int TOTAL_PIXELS_S = 16 * 16 * 256;
    int TOTAL_PIXELS_B = 512 * 288 * 4;

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

    for(int i = 0; i < TOTAL_PIXELS_B; i++) { 
        B_DATA[i] = (uint8_t)spriteData[TOTAL_COLORS + TOTAL_PIXELS_L + TOTAL_PIXELS_M + TOTAL_PIXELS_S + i];
    }
};

uint8_t drawSprites(uint16_t x, uint16_t y, uint16_t z, uint16_t index, uint16_t type, uint16_t palette, uint32_t controlStructure) {
    volatile uint32_t *L_CONTROL = (volatile uint32_t *)(0x500F5B00);
    volatile uint32_t *M_CONTROL = (volatile uint32_t *)(0x500F5F00);
    volatile uint32_t *S_CONTROL = (volatile uint32_t *)(0x500F6300);
    volatile uint32_t *B_CONTROL = (volatile uint32_t *)(0x500F5A00);
    volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);

    volatile uint32_t *control;
    if(type == 0) {
        control = L_CONTROL;
    }
    else if(type == 1) {
        control = M_CONTROL;
    }
    else if(type == 2) {
        control = S_CONTROL;
    } 
    else if(type == 3) {
        control = B_CONTROL;
    } 
    else {
        return -1;
    }

    control[controlStructure] = (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | (((uint32_t)y)<<12) | (((uint32_t)x)<<2) | (palette & 0x3);

    *MODE_REGISTER = 1;
}


// uint32_t MediumControl(uint8_t palette, int16_t x, int16_t y, uint8_t z, uint8_t index){
//     return (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | (((uint32_t)y+32)<<12) | (((uint32_t)x+32)<<2) | (palette & 0x3);
// }