#include "./graphics.h"

uint8_t drawSpritesHelper(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t controlStructure);

uint8_t drawSprite(uint16_t x, uint16_t y, uint16_t z, uint16_t index, uint16_t type, uint16_t palette, uint32_t controlStructure) {

    uint32_t arg0 = (((uint32_t)x)<<16) | (((uint32_t)y)&0xFFFF);
    uint32_t arg1 = (((uint32_t)z)<<16) | (((uint32_t)index)&0xFFFF);
    uint32_t arg2 = (((uint32_t)type)<<16) | (((uint32_t)palette)&0xFFFF);

    drawSpritesHelper(arg0, arg1, arg2, controlStructure);
}