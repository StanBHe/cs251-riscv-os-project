#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "./syscalls/events.h"
#include "./syscalls/graphics.h"
#include "./syscalls/input.h"
#include "./syscalls/state.h"
#include "./syscalls/threads.h"
#include "./syscalls/time.h"
#include "./usefulValues.h"
#include "./spriteData.h"

TThreadContext other_thread;
TThreadContext MainThread;

volatile int last_global = 0;
volatile int global = 0;
volatile int controller_status = 0;
volatile int last_reset = 0;
volatile int reset;
volatile int rowsDeleted = 0;

int SCORE_X_POS = 210;
int SCORE_Y_POS = 20;

int BLOCK_SIZE = 8;
int GRID_OFFSET_X = 207;
int GRID_OFFSET_Y = 80;

int GRID_WIDTH = 10;
int GRID_HEIGHT = 24;

int TIME_STEP = 20;

int QUEUE_SIZE = 5;

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

int getRotateOffset(int x, int y, int type, int rot);

int** getbMap(int x, int y, int type, int rot);

void freebMap(int** bMap);

void rotatebMap(int** bMap);

int isValid(int x, int y, int type, int rot, int** grid);

int placeBlock(int x, int y, int type, int rot, int** grid);

int printGrid(int** grid);

void drop(int x, int y, int type, int rot, int** grid);

void drawGrid(int** grid);

void deleteGridRows(int** grid);

void printNumber(int x, int y, int count);

void drawBlockQueue(int* queue);

void drawNumber(int x, int y, int count);

int main() {
    last_reset = getReset();

    loadSprites(SPRITE_DATA);
    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
    setGraphicsMode(GRAPHICS_MODE);
    drawSprite(0, 0, 0, 0, BACKGROUND_T, 0, 0);

    // game var
    int gameState = 0;
    int scoreloop = 0;
    int bType = 0;
    int bRot = 0;
    int bX = 5;
    int bY = 0;
    int pressedLR = 0;
    int pressedD = 0;
    int pressedU = 0;
    int pressedRot = 0;
    int diff = 0;
    int blockCount = 0;
    int aC_xPos = 30;
    int aC_yPos = 120;
    int xC_xPos = 60;
    int xC_yPos = 200;
    int dC_xPos = 90;
    int dC_yPos = 120;

    int** grid = (int**)malloc(sizeof(int*) * GRID_HEIGHT);
    for(int i = 0; i < GRID_HEIGHT; i++) {
        grid[i] = (int*)malloc(sizeof(int) * GRID_WIDTH);
        for(int k = 0; k < GRID_WIDTH; k++) {
            grid[i][k] = -1;
        }
    }

    int* blockQueue = (int*)malloc(sizeof(int) * QUEUE_SIZE);
    for(int i = 0; i < QUEUE_SIZE; i++) {
        blockQueue[i] = i;
    }

    while (1) {
        reset = getReset();
        global= getTicks();
        controller_status = getController();

        if(gameState == 0) {
            if(global != last_global){
                if(controller_status){
                    if(controller_status & U_KEY){
                        aC_xPos = 30;
                        aC_yPos = 120;
                        xC_xPos = 60;
                        xC_yPos = 200;
                        dC_xPos = 90;
                        dC_yPos = 120;
                        gameState = 1;
                        drawSprite(0, 0, 0, 1, BACKGROUND_T, 0, 0);
                        drawSprite(340, 100, 1, 0, LARGE_T, 0, 0);
                        drawSprite(410, 100, 1, 1, LARGE_T, 0, 1);
                        drawSprite(aC_xPos, aC_yPos, 1, 2, LARGE_T, 0, 2);
                        drawSprite(xC_xPos, xC_yPos, 1, 3, LARGE_T, 0, 3);
                        drawSprite(dC_xPos, dC_yPos, 1, 4, LARGE_T, 0, 4);
                        drawGrid(grid);
                        drawBlockQueue(blockQueue);
                        printGrid(grid);
                        drawNumber(SCORE_X_POS, SCORE_Y_POS, 0);
                    }
                }
            }
        }

        if(gameState == 2) {
            if(controller_status) {
                if(controller_status & K_KEY){
                    setGraphicsMode(GRAPHICS_MODE);
                    drawSprite(0, 0, 2, 0, BACKGROUND_T, 0, 0);
                    rowsDeleted = 0;
                    blockCount = 0;
                    gameState = 0;
                    for(int i = 0; i < GRID_HEIGHT; i++) {
                        for(int k = 0; k < GRID_WIDTH; k++) {
                            grid[i][k] = -1;
                        }
                    }
                    drawGrid(grid);
                    clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
                }
            }
        }

        if(gameState == 1) {
            if(global != last_global){
                diff = global - last_global;
                controller_status = getController();
                if(controller_status & A_KEY) {
                    if(isValid(bX-1, bY, bType, bRot, grid) && (pressedLR==0 || pressedLR >= TIME_STEP)) {
                        bX--;
                    }
                    if(pressedLR >= TIME_STEP) {
                        pressedLR -= TIME_STEP;
                    }
                    pressedLR += diff;
                }
                else if(controller_status & D_KEY) {
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
                if(controller_status & W_KEY){
                }
                if(controller_status & X_KEY){
                    if(pressedD==0 || pressedD >= TIME_STEP) {
                        if(isValid(bX, bY+1, bType, bRot, grid)) {
                            bY++;
                        }
                        else {
                            if(placeBlock(bX, bY, bType, bRot, grid)) {
                                bRot = 0;
                                bX = 5;
                                bY = 0;
                                blockCount++;
                                bType = blockQueue[0];
                                for(int i = 0; i < QUEUE_SIZE-1; i++) {
                                    blockQueue[i] = blockQueue[i+1];
                                }
                                blockQueue[QUEUE_SIZE-1] = global % 7;
                                
                                drawBlockQueue(blockQueue);
                                deleteGridRows(grid);
                                printNumber(2, 20, blockCount);
                                printGrid(grid);
                                drawGrid(grid);
                                drawNumber(SCORE_X_POS, SCORE_Y_POS, (rowsDeleted * 40) + (blockCount * 5));
                            }
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
                if(controller_status & U_KEY) {
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
                else if(controller_status & I_KEY) {
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
                if(controller_status & J_KEY){
                    setGraphicsMode(TEXT_MODE);
                }
                if(controller_status & K_KEY){
                    setGraphicsMode(GRAPHICS_MODE);
                }
                if(global % (TIME_STEP * 10) == 0) {
                    if(isValid(bX, bY+1, bType, bRot, grid)) {
                        bY++;
                    }
                    else {
                        if(placeBlock(bX, bY, bType, bRot, grid)) {
                            bRot = 0;
                            bX = 5;
                            bY = 0;
                            blockCount++;
                            bType = blockQueue[0];
                            for(int i = 0; i < QUEUE_SIZE-1; i++) {
                                blockQueue[i] = blockQueue[i+1];
                            }
                            blockQueue[QUEUE_SIZE-1] = global % 7;
                            
                            drawBlockQueue(blockQueue);
                            deleteGridRows(grid);
                            printNumber(2, 20, blockCount);
                            printGrid(grid);
                            drawGrid(grid);
                            drawNumber(SCORE_X_POS, SCORE_Y_POS, (rowsDeleted * 40) + (blockCount * 5));
                        } else {
                            // game over
                            clearTextArea(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
                            drawText(5, 5, "Game Over");
                            drawText(5, 10, "Blocks Placed:");
                            printNumber(5, 12, blockCount);
                            drawText(5, 16, "Lines Cleared:");
                            printNumber(5, 18, rowsDeleted);
                            drawText(5, 22, "Final Score:");
                            printNumber(5, 24, (rowsDeleted * 40) + (blockCount * 5));
                            drawText(5, 30, "Press K to return to start screen");
                            setGraphicsMode(TEXT_MODE);
                            gameState = 2;
                        }
                    }
                }
                if(global % (TIME_STEP * 2) == 0) {
                    aC_xPos--;
                    if(aC_xPos < 20) {
                        aC_xPos = 30;
                    }
                    xC_yPos++;
                    if(xC_yPos > 210) {
                        xC_yPos = 200;
                    }
                    dC_xPos++;
                    if(dC_xPos > 100) {
                        dC_xPos = 90;
                    }
                    drawSprite(aC_xPos, aC_yPos, 1, 2, LARGE_T, 0, 2);
                    drawSprite(xC_xPos, xC_yPos, 1, 3, LARGE_T, 0, 3);
                    drawSprite(dC_xPos, dC_yPos, 1, 4, LARGE_T, 0, 4);
                }
            }
            drawBlock(bX, bY, bType, bRot, 0);
        }

        last_global = global;
        if(last_reset != reset){
            last_reset = reset;
            for(int i = 0; i < GRID_HEIGHT; i++) {
                for(int k = 0; k < GRID_WIDTH; k++) {
                    grid[i][k] = -1;
                }
            }
            printGrid(grid);
            drawGrid(grid);
        }
    }
}

// tetris logic functions

int** getbMap(int x, int y, int type, int rot) {
    int** bMap = (int**)malloc(sizeof(int*) * 4);
    if(!bMap) {
        drawText(2, 22, "failed to alloc bmap");
    }
    for(int i = 0; i < 4; i++) {
        bMap[i] = (int*)malloc(sizeof(int) * 4);
        if(!bMap[i]) {
            drawText(2, 24, "failed to alloc bmap row");
        }
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
                if(grid[i+y][k+x] != -1) {
                    freebMap(bMap);
                    return 0;
                }
            }
        }
    }

    freebMap(bMap);
    return 1;
}

int placeBlock(int x, int y, int type, int rot, int** grid) {
    if(!isValid(x, y, type, rot, grid)) {
        // game over
        return 0;
    }
    int** bMap = getbMap(x, y, type, rot);
    printBlock(bMap);
    for(int i = 0; i < 4; i++) {
        for(int k = 0; k < 4; k++) {
            if(bMap[i][k] == 1) {
                grid[i+y][k+x] = type;  
            }
        }
    }
    freebMap(bMap);
    return 1;
}

void deleteGridRows(int** grid) {
    for(int i = 0; i < GRID_HEIGHT; i++) {
        int isRow = 1;
        for(int k = 0; k < GRID_WIDTH; k++) {
            if(grid[i][k] == -1) {
                isRow = 0;
            }
        }
        if(isRow == 1) {
            rowsDeleted++;
            printNumber(2, 22, rowsDeleted);
            for(int k = i - 1; k > 0; k--) {
                for(int j = 0; j < GRID_WIDTH; j++) {
                    grid[k+1][j] = grid[k][j];
                }
            }
            for(int j = 0; j < GRID_WIDTH; j++) {
                grid[0][j] = -1;
            }
        }
    }
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

// drawing functions

void drawBlock(int x, int y, int type, int rot, int index) {
    drawSprite(GRID_OFFSET_X + (x * BLOCK_SIZE), GRID_OFFSET_Y + (y * BLOCK_SIZE), 1, (8 * type) + rot, 1, 0, index);
}

void drawGrid(int** grid) {
    for(int i = 0; i < GRID_HEIGHT; i++) {
        for(int k = 0; k < GRID_WIDTH; k++) {
            if(grid[i][k] != -1) {
                drawSprite(GRID_OFFSET_X + (k * BLOCK_SIZE), GRID_OFFSET_Y + (i * BLOCK_SIZE), 2, 
                           grid[i][k], 2, 0, i + (k * GRID_HEIGHT));
            } else {
                clearSprite(2, i + (k * GRID_HEIGHT));
            }
        }
    }
}

void drawBlockQueue(int* queue) {
    int offset = 0;
    for(int i = 0; i < QUEUE_SIZE; i++) {
        int rot = 0;
        if(BLOCK_SIZES[queue[i]][0] == 3) {
            rot = 1;
        }
        drawSprite(300, GRID_OFFSET_Y + offset, 1, (8 * queue[i]) + rot, 1, 0, i + 1);
        if(BLOCK_SIZES[queue[i]][0] == 3 || BLOCK_SIZES[queue[i]][1] == 3) {
            offset += 8;
        } else if(queue[i] == 0) {
            offset += 16;
        }
        offset += 24;
    }
}

// text mode functions

void printBlock(int** bmap) {
    for(int i = 0; i < 4; i++) {
        for(int k = 0; k < 4; k++) {
            if(bmap[i][k] == 0) {
                drawText(1 + 2*k, 1 + 2*i, " ");
            } else {
                drawText(1 + 2*k, 1 + 2*i, "*");
            }
        }
    }
}

int printGrid(int** grid) {
    for(int i = 0; i < GRID_HEIGHT; i++) {
        char* lineText = malloc(sizeof(char) * (GRID_WIDTH + 3));
        lineText[0] = 'X';
        lineText[GRID_WIDTH + 1] = 'X';
        lineText[GRID_WIDTH + 2] = '\0';
        for(int k = 0; k < GRID_WIDTH; k++) { 
            if(grid[i][k] == -1) {
                lineText[k+1] = ' ';
            } else {
                lineText[k+1] = (char)(grid[i][k] + 48);
            }
        }
        drawText(25, 10 + i, lineText);
        free(lineText);
    }
    drawText(25, 10 - 1, "XXXXXXXXXXXX");
    drawText(25, 10 + GRID_HEIGHT, "XXXXXXXXXXXX");
}

int NUM_DIGITS = 8;

void printNumber(int x, int y, int count) {
    int c = count;
    char* lineText = malloc(sizeof(char) * (NUM_DIGITS+1));
    lineText[NUM_DIGITS] = '\0';
    for(int i = 0; i < NUM_DIGITS; i++) {
        lineText[NUM_DIGITS-1-i] = (char)((c % 10) + 48);
        c /= 10;
    }
    drawText(x, y, lineText);
    free(lineText);
}

void drawNumber(int x, int y, int count) {
    int c = count;
    for(int i = 0; i < NUM_DIGITS; i++) {
        drawSprite((MEDIUM_SIZE * (NUM_DIGITS - 1)) + x - (MEDIUM_SIZE * i), y, 1, (c % 10) + 54, MEDIUM_T, 0, 10 + i);
        c /= 10;
    }
}
