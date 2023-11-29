#ifndef SPRITES_H
#define SPRITES_H

#include "stdint.h"

void loadSprites(uint32_t* spriteData);

int drawSprites(int x, int y, int z, int palette, int index);

#endif 