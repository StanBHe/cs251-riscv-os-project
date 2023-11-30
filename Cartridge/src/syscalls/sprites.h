#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

void loadSprites(uint32_t spriteData[]);

uint8_t drawSprites(uint16_t x, uint16_t y, uint16_t z, uint16_t index, uint16_t type, uint16_t palette, uint32_t controlStructure);

uint8_t drawSpritesHelper(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t controlStructure);

#endif 