#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

void loadSprites(uint32_t spriteData[]);

int8_t drawSprite(uint16_t x, uint16_t y, uint16_t z, uint16_t index, uint16_t type, uint16_t palette, uint32_t controlStructure);

int8_t clearSprite(uint16_t type, uint32_t controlStructure);

int32_t drawText(uint16_t x, uint16_t y, char* text);

int8_t clearText(int32_t textPos);

int8_t clearTextArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

int8_t setGraphicsMode(uint8_t m);

#endif 