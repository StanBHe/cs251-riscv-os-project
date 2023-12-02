#include "graphics.h"

uint16_t SCREEN_WIDTH = 512;
uint16_t SCREEN_HEIGHT = 288;

uint16_t TEXT_WIDTH = 64;
uint16_t TEXT_HEIGHT = 36;

volatile char *VIDEO_MEMORY = (volatile char *)(0x500F4800);

volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);
volatile uint32_t *PALETTE = (volatile uint32_t *)(0x500F0000);
volatile uint8_t *L_DATA = (volatile uint8_t *)(0x50090000);
volatile uint8_t *M_DATA = (volatile uint8_t *)(0x500D0000);
volatile uint8_t *S_DATA = (volatile uint8_t *)(0x500E0000);
volatile uint8_t *B_DATA1 = (volatile uint8_t *)(0x50000000);
volatile uint8_t *B_DATA2 = (volatile uint8_t *)(0x50024000);
volatile uint8_t *B_DATA3 = (volatile uint8_t *)(0x50048000);
volatile uint8_t *B_DATA4 = (volatile uint8_t *)(0x5006C000);

volatile uint32_t *L_CONTROL = (volatile uint32_t *)(0x500F5B00);
volatile uint32_t *M_CONTROL = (volatile uint32_t *)(0x500F5F00);
volatile uint32_t *S_CONTROL = (volatile uint32_t *)(0x500F6300);
volatile uint32_t *B_CONTROL = (volatile uint32_t *)(0x500F5A00);

uint32_t TOTAL_COLORS = 256 * 4 * 4;
uint32_t TOTAL_PIXELS_L = 64 * 64 * 64;
uint32_t TOTAL_PIXELS_M = 32 * 32 * 64;
uint32_t TOTAL_PIXELS_S = 16 * 16 * 256;
uint32_t TOTAL_PIXELS_B = 512 * 288;

void loadSprites(uint32_t spriteData[]) {

    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
    drawText(1, 1, "Loading Sprites...");

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
        B_DATA1[i] = (uint8_t)spriteData[TOTAL_COLORS + TOTAL_PIXELS_L + TOTAL_PIXELS_M + TOTAL_PIXELS_S + (TOTAL_PIXELS_B * 0) + i];
    }

    for(int i = 0; i < TOTAL_PIXELS_B; i++) { 
        B_DATA2[i] = (uint8_t)spriteData[TOTAL_COLORS + TOTAL_PIXELS_L + TOTAL_PIXELS_M + TOTAL_PIXELS_S + (TOTAL_PIXELS_B * 1) + i];
    }

    for(int i = 0; i < TOTAL_PIXELS_B; i++) { 
        B_DATA3[i] = (uint8_t)spriteData[TOTAL_COLORS + TOTAL_PIXELS_L + TOTAL_PIXELS_M + TOTAL_PIXELS_S + (TOTAL_PIXELS_B * 2) + i];
    }

    for(int i = 0; i < TOTAL_PIXELS_B; i++) { 
        B_DATA4[i] = (uint8_t)spriteData[TOTAL_COLORS + TOTAL_PIXELS_L + TOTAL_PIXELS_M + TOTAL_PIXELS_S + (TOTAL_PIXELS_B * 3) + i];
    }

    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
};

int8_t drawSprite(uint16_t x, uint16_t y, uint16_t z, uint16_t index, uint16_t type, uint16_t palette, uint32_t controlStructure) {
    volatile uint32_t *control;
    uint32_t size = 0;
    if(type == 0) {
        control = L_CONTROL;
        size = 64;
    }
    else if(type == 1) {
        control = M_CONTROL;
        size = 32;
    }
    else if(type == 2) {
        control = S_CONTROL;
        size = 16;
    } 
    else if(type == 3) {
        control = B_CONTROL;
        control[controlStructure] = ((((uint32_t)index)<<29) | (((uint32_t)z)<<22) | 
                                    (((uint32_t)(y+SCREEN_HEIGHT))<<12) | (((uint32_t)(x+SCREEN_WIDTH))<<2) | (palette & 0x3));
        return 0;
    } 
    else {
        return -1;
    }

    control[controlStructure] = (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | 
                                (((uint32_t)(y+size))<<12) | (((uint32_t)(x+size))<<2) | (palette & 0x3);

    return 0;
}

int8_t clearSprite(uint16_t type, uint32_t controlStructure) {
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
    control[controlStructure] = 0;
    return 0;
}


int32_t drawText(uint16_t x, uint16_t y, char* text) {
    uint32_t shift = (y * TEXT_WIDTH) + x;
    uint32_t strLen = strlen(text);

    /* check that string fits */
    if(shift + strLen >= TEXT_WIDTH * TEXT_HEIGHT) {
        return -1;
    }

    volatile char *ADDR = (volatile char *)(&VIDEO_MEMORY[shift]);
    strcpy((char *)ADDR, (char*)text);

    return shift;
}

int8_t clearText(int32_t textPos) {

    if(textPos < 0) {
        return -1;
    }

    int32_t pos = textPos;

    while(VIDEO_MEMORY[pos] != '\0' && pos < (TEXT_WIDTH * TEXT_HEIGHT)) {
        VIDEO_MEMORY[pos] = '\0';
        pos++;
    }

    return 0;
}

int8_t clearTextArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

    uint16_t right = x + w;
    if(right > TEXT_WIDTH) {
        right = TEXT_WIDTH;
    }

    for(uint16_t i = 0; i < h; i++) {
        uint32_t vshift = ((y + i) * TEXT_WIDTH);
        for(uint16_t k = x; k < right; k++) {
            VIDEO_MEMORY[vshift + k] = 0;
        }
    }

    return 0;
}

int8_t setGraphicsMode(uint8_t m) {
    if(m > 1) {
        return -1;
    }
    *MODE_REGISTER = m;
    return 0;
}