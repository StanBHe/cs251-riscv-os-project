#include <stdint.h>
#include <time.h>
#include "./syscalls/input.h"
#include "./syscalls/state.h"
#include "./spriteData.h"
#include "./syscalls/graphics.h"
#include "./syscalls/memory.h"

uint32_t SCREEN_WIDTH = 512;
uint32_t SCREEN_HEIGHT = 288;

uint32_t TEXT_WIDTH = 64;
uint32_t TEXT_HEIGHT = 36;

int BLOCK_SIZE = 8;
int GRID_OFFSET_X = 207;
int GRID_OFFSET_Y = 80;

int GRID_WIDTH = 10;
int GRID_HEIGHT = 24;

int TIME_STEP = 10;

// unrotated block sizes width, height
int BLOCK_SIZES[7][2] = {{1,4}, {2,3}, {2,3}, {3,2}, {3,2}, {3,2}, {2,2}};

int BLOCK_ROT_MODS[7] = {2, 4, 4, 2, 2, 4, 1};

int INIT_BLOCK_STRUCTURES[7][4][4] = {
                                   {{1,0,0,0}, 
                                    {1,0,0,0}, 
                                    {1,0,0,0},
                                    {1,0,0,0}},
                                    
                                   {{1,0,0,0}, 
                                    {1,0,0,0}, 
                                    {1,1,0,0},
                                    {0,0,0,0}},
                                    
                                   {{0,1,0,0}, 
                                    {0,1,0,0}, 
                                    {1,1,0,0},
                                    {0,0,0,0}},
                                    
                                   {{0,1,1,0}, 
                                    {1,1,0,0}, 
                                    {0,0,0,0},
                                    {0,0,0,0}},
                                    
                                   {{1,1,0,0}, 
                                    {0,1,1,0}, 
                                    {0,0,0,0},
                                    {0,0,0,0}},
                                    
                                   {{0,1,0,0}, 
                                    {1,1,1,0}, 
                                    {0,0,0,0},
                                    {0,0,0,0}},
                                    
                                   {{1,1,0,0}, 
                                    {1,1,0,0}, 
                                    {0,0,0,0},
                                    {0,0,0,0}}};

void drawBlock(int x, int y, int type, int rot, int index);

// returns x position shift if rotated block moved
int getRotateOffset(int x, int y, int type, int rot);

int** getbMap(int x, int y, int type, int rot);

void freebMap(int** bMap);

void rotatebMap(int** bMap);

int isValid(int x, int y, int type, int rot, int** grid);

void placeBlock(int x, int y, int type, int rot, int** grid);

int printGrid(int** grid);

void drop(int x, int y, int type, int rot, int** grid);

void drawGrid(int** grid);

/*
    drawSprites:
    x, y, z, sprite_index, sprite_type, palette, control structure_index
    sprite types: 0 = large, 1 = medium, 2 = small, 3 = background

    Controller Mapping:
          w=02             u=10  i=20
    a=01        d=08  cmd  
          x=04             j=40  k=80

    Block types:
*/

int main() {

    loadSprites(SPRITE_DATA);
    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
    setGraphicsMode(1);
    drawSprite(0, 0, 0, 0, 3, 0, 0);

    int last_reset = GetReset();
    int reset;
    int last_global = 0;

    uint32_t global = 0;
    uint32_t controller_status = 0;

    // game var
    int menuloop = 1;
    int gameloop = 0;
    int scoreloop = 0;

    int bInd = 0;
    int bType = 0;
    int bRot = 0;
    int bX = 0;
    int bY = 0;

    int pressedLR = 0;
    int pressedD = 0;
    int pressedRot = 0;
    int diff = 0;

    int** grid = (int**)malloc(sizeof(int*) * GRID_HEIGHT);
    for(int i = 0; i < GRID_HEIGHT; i++) {
        grid[i] = (int*)malloc(sizeof(int) * GRID_WIDTH);
        for(int k = 0; k < GRID_WIDTH; k++) {
            grid[i][k] = 0;
        }
    }

    while (1) {
        reset = GetReset();
        global= GetTicks();

        if(menuloop) {
            if(global != last_global){
                controller_status = GetController();
                if(controller_status){
                    if(controller_status & 0x10){
                        gameloop = 1;
                        menuloop = 0;
                        drawSprite(0, 0, 0, 1, 3, 0, 0);
                    }
                }
            }
        }

        if(gameloop) {
            if(global != last_global){
                diff = global - last_global;
                controller_status = GetController();
                if(controller_status & 0x1) {
                    if(isValid(bX-1, bY, bType, bRot, grid) && (pressedLR==0 || pressedLR >= TIME_STEP)) {
                        bX--;
                    }
                    if(pressedLR >= TIME_STEP) {
                        pressedLR -= TIME_STEP;
                    }
                    pressedLR += diff;
                }
                else if(controller_status & 0x8) {
                    if(isValid(bX+1, bY, bType, bRot, grid) && (pressedLR==0 || pressedLR >= TIME_STEP)) {
                        bX++;
                    }
                    if(pressedLR >= TIME_STEP) {
                        pressedLR -= TIME_STEP;
                    }
                    pressedLR += diff;
                }
                else {
                    pressedLR = 0;
                }
                if(controller_status & 0x2){
                }
                if(controller_status & 0x4){
                    if(pressedD==0 || pressedD >= TIME_STEP) {
                        if(isValid(bX, bY+1, bType, bRot, grid)) {
                            bY++;
                        }
                        else {
                            placeBlock(bX, bY, bType, bRot, grid);
                            bType = global % 7;
                            bRot = 0;
                            bX = 0;
                            bY = 0;
                        }
                    }
                    if(pressedD >= TIME_STEP) {
                        pressedD -= TIME_STEP;
                    }
                    pressedD += diff;
                }
                else {
                    pressedD = 0;
                }
                if(controller_status & 0x10){
                    if(pressedRot == 0) {
                        int offset = 0;
                        offset = getRotateOffset(bX, bY, bType, bRot);
                        bX += offset;
                        bRot = (bRot + 3) % BLOCK_ROT_MODS[bType];
                        if(!isValid(bX, bY, bType, bRot, grid)) {
                            bX -= offset;
                            bRot = (bRot + 1) % BLOCK_ROT_MODS[bType];
                        }
                        pressedRot = 1;
                    }
                }
                else if(controller_status & 0x20){
                    if(pressedRot == 0) {
                        int offset = 0;
                        offset = getRotateOffset(bX, bY, bType, bRot);
                        bX += offset;
                        bRot = (bRot + 1) % BLOCK_ROT_MODS[bType];
                        if(!isValid(bX, bY, bType, bRot, grid)) {
                            bX -= offset;
                            bRot = (bRot + 3) % BLOCK_ROT_MODS[bType];
                        }
                        pressedRot = 1;
                    }
                }
                else {
                    pressedRot = 0;
                }
                if(controller_status & 0x40){
                    setGraphicsMode(0);
                }
                if(controller_status & 0x80){
                    setGraphicsMode(1);
                }
            }
            drawBlock(bX, bY, bType, bRot, bInd);

        }

        if(global % 10 == 0) {
            printGrid(grid);
            drawGrid(grid);
        }

        last_global = global;
        if(last_reset != reset){
            last_reset = reset;
            gameloop = 0;
            menuloop = 1;
            drawSprite(512, 288, 0, 0, 3, 0, 0);
        }
    }

    return 0;
}


void drawBlock(int x, int y, int type, int rot, int index) {
    drawSprite(GRID_OFFSET_X + (x * BLOCK_SIZE), GRID_OFFSET_Y + (y * BLOCK_SIZE), 1, (8 * type) + rot, 1, 0, index);
}

int** getbMap(int x, int y, int type, int rot) {
    int** bMap = (int**)malloc(sizeof(int*) * 4);
    for(int i = 0; i < 4; i++) {
        bMap[i] = (int*)malloc(sizeof(int) * 4);
        for(int k = 0; k < 4; k++) {
            bMap[i][k] = 0;
        }
    }

    for(int i = 0; i < 4; i++) {
        for(int k = 0; k < 4; k++) {
            bMap[i][k] = INIT_BLOCK_STRUCTURES[type][i][k];
        }
    }

    if(type == 0) {
        if(rot % 2 == 1) {
            for(int i = 0; i < 4; i++) {
                bMap[i][0] = 0;
                bMap[0][i] = 1;
            }
        }
    } else {
        for(int i = 0; i < rot; i++) {
            rotatebMap(bMap);
        }
    }

    return bMap;
}

void freebMap(int** bMap) {
    for(int i = 0; i < 4; i++) {
        free(bMap[i]);
    }
    free(bMap);
}

int isValid(int x, int y, int type, int rot, int** grid) {
    if(x < 0 || y < 0) {
        return 0;
    }

    int** bMap = getbMap(x, y, type, rot);

    for(int i = 0; i < 4; i++) {
        for(int k = 0; k < 4; k++) {
            if(bMap[i][k]==1) {
                if(x + k + 1 > GRID_WIDTH) {
                    freebMap(bMap);
                    return 0;
                }
                if(y + i + 1 > GRID_HEIGHT) {
                    freebMap(bMap);
                    return 0;
                }
                if(grid[i+y][k+x] == 2) {
                    freebMap(bMap);
                    return 0;
                }
            }
        }
    }

    freebMap(bMap);
    return 1;
}

void placeBlock(int x, int y, int type, int rot, int** grid) {
    int** bMap = getbMap(x, y, type, rot);
    for(int i = 0; i < 4; i++) {
        for(int k = 0; k < 4; k++) {
            if(bMap[i][k] == 1) {
                grid[i+y][k+x] = 2;
            }
        }
    }
    freebMap(bMap);
}

void rotatebMap(int** bMap) {

    int temp1 = bMap[0][0];
    bMap[0][0] = bMap[2][0];
    bMap[2][0] = bMap[2][2];
    bMap[2][2] = bMap[0][2];
    bMap[0][2] = temp1;

    int temp2 = bMap[0][1];
    bMap[0][1] = bMap[1][0];
    bMap[1][0] = bMap[2][1];
    bMap[2][1] = bMap[1][2];
    bMap[1][2] = temp2;

    if(bMap[0][0] + bMap[1][0] + bMap[2][0] + bMap[3][0] == 0) {
        for(int i = 0; i < 3; i++) {
            int temp = bMap[i][0];
            bMap[i][0] = bMap[i][1];
            bMap[i][1] = bMap[i][2];
            bMap[i][2] = temp;
        }
    }
    else if(bMap[0][0] + bMap[0][1] + bMap[0][2] + bMap[0][3] == 0) {
        int* temp = bMap[0];
        bMap[0] = bMap[1];
        bMap[1] = bMap[2];
        bMap[2] = temp;
    }
}

int getRotateOffset(int x, int y, int type, int rot) {
    if((x + BLOCK_SIZES[type][(rot+1)%2]) > GRID_WIDTH) {
        if(type == 0) {
            return GRID_WIDTH - (x + BLOCK_SIZES[type][(rot+1)%2]);
        }
        else {
            return -1;
        }
    }
    return 0;
}

int printGrid(int** grid) {
    for(int i = 0; i < GRID_HEIGHT; i++) {
        char* lineText = malloc(sizeof(char) * (GRID_WIDTH + 3));
        lineText[0] = 'X';
        lineText[GRID_WIDTH + 1] = 'X';
        lineText[GRID_WIDTH + 2] = '\0';
        for(int k = 0; k < GRID_WIDTH; k++) { 
            if(grid[i][k] == 0) {
                lineText[k+1] = '0';
            } else {
                lineText[k+1] = '2';
            }
        }
        drawText(25, 10 + i, lineText);
        free(lineText);
    }
    drawText(25, 10 - 1, "XXXXXXXXXXXX");
    drawText(25, 10 + GRID_HEIGHT, "XXXXXXXXXXXX");
}

void drawGrid(int** grid) {
    for(int i = 0; i < GRID_HEIGHT; i++) {
        for(int k = 0; k < GRID_WIDTH; k++) {
            if(grid[i][k] != 0) {
                drawSprite(GRID_OFFSET_X + (k * BLOCK_SIZE), GRID_OFFSET_Y + (i * BLOCK_SIZE), 2, 
                           grid[i][k] - 1, 2, 0, i + (k * GRID_HEIGHT));
            }
        }
    }
}