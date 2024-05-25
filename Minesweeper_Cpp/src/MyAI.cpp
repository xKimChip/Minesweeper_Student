// ======================================================================
// FILE:        MyAI.cpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#include "MyAI.hpp"

void printTensor(int8_t *tensor, int height, int width, const char *name) {
    printf("%s:\n", name);
    for (uint32_t y = height; y--;) {
        for (uint32_t x = 0; x < width; x++) {
            printf("%d ", tensor[y * width + x]);
        }
        printf("\n");
    }
    printf("\n");
}

void printTensor2(uint32_t *tensor, int width, const char *name) {
    printf("%s:\n", name);
    for (uint32_t x = 0; x < width; x++) {
        printf("%d ", tensor[x]);
    }
    printf("\n");
}

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines = _totalMines;
    agentX = _agentX;
    agentY = _agentY;
    pIdx = agentY * colDimension + agentX;
    
    neighboringMines = new int8_t[rowDimension * colDimension]{};
    unexploredNeighbors = new int8_t[rowDimension * colDimension];
    for (uint32_t y = rowDimension; y--;) {
        for (uint32_t x = colDimension; x--;) {
            switch(((y == 0) || (y == rowDimension - 1)) + ((x == 0) || (x == colDimension - 1))) {
                case 0: unexploredNeighbors[y * colDimension + x] = 8; break;
                case 1: unexploredNeighbors[y * colDimension + x] = 5; break;
                case 2: unexploredNeighbors[y * colDimension + x] = 3; break;
            }
        }
    }
    wasSafe = new int8_t[rowDimension * colDimension]{};
    
    safeQueue = new uint32_t[rowDimension * colDimension]{};
    safeQStartIdx = 0;
    safeQEndIdx = 0;
    updateNMQueue = new uint32_t[rowDimension * colDimension * 9]{};
    updateNMStartIdx = 0;
    updateNMEndIdx = 0;
    updateUNQueue = new uint32_t[rowDimension * colDimension * 9]{};
    updateUNStartIdx = 0;
    updateUNEndIdx = 0;
    
    wasSafe[pIdx] = 1;
};

MyAI::~MyAI ()
{
    delete[] neighboringMines;
    delete[] unexploredNeighbors;
    delete[] wasSafe;
    delete[] safeQueue;
    delete[] updateNMQueue;
    delete[] updateUNQueue;
};

Agent::Action MyAI::getAction( int number )
{
    int8_t dy, dx, ddy, ddx;
    uint32_t tmpIdx;
    
    neighboringMines[pIdx] += number;
    updateNMQueue[updateNMEndIdx++] = pIdx;
    
    for (dy = -1; dy <= 1; dy++) {
        if (agentY < -dy || agentY >= colDimension - dy) continue;
        for (dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            if (agentX < -dx || agentX >= rowDimension - dx) continue;
            uint32_t tmpIdx = (agentY + dy) * colDimension + agentX + dx;
            unexploredNeighbors[tmpIdx]--;
            updateUNQueue[updateUNEndIdx++] = tmpIdx;
        }
    }
    
    uint8_t NM = updateNMStartIdx < updateNMEndIdx;
    uint32_t updateIdx;
    int32_t updateY, updateX;
    while (NM || updateUNStartIdx < updateUNEndIdx) {
        if (NM) updateIdx = updateNMQueue[updateNMStartIdx++];
        else updateIdx = updateUNQueue[updateUNStartIdx++];
        updateY = updateIdx / colDimension;
        updateX = updateIdx - updateY * colDimension;
        if (NM && neighboringMines[updateIdx] == 0) {
            for (dy = -1; dy <= 1; dy++) {
                if (updateY < -dy || updateY >= rowDimension - dy) continue;
                for (dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    if (updateX < -dx || updateX >= colDimension - dx) continue;
                    tmpIdx = (updateY + dy) * colDimension + updateX + dx;
                    if (wasSafe[tmpIdx] == 1) continue;
                    wasSafe[tmpIdx] = 1;
                    safeQueue[safeQEndIdx++] = tmpIdx;
                    //printf("safe at %d %d\n", updateX + dx, updateY + dy);
                }
            }
        } else if (neighboringMines[updateIdx] == unexploredNeighbors[updateIdx]) {
            for (dy = -1; dy <= 1; dy++) {
                if (updateY < -dy || updateY >= rowDimension - dy) continue;
                for (dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    if (updateX < -dx || updateX >= colDimension - dx) continue;
                    tmpIdx = (updateY + dy) * colDimension + updateX + dx;
                    if (wasSafe[tmpIdx] == 1) continue;
                    wasSafe[tmpIdx] = 1;
                    //printf("mine at %d %d\n", updateX + dx, updateY + dy);
                    
                    for (ddy = -1; ddy <= 1; ddy++) {
                        if (updateY + dy < -ddy || updateY + dy >= rowDimension - ddy) continue;
                        for (ddx = -1; ddx <= 1; ddx++) {
                            if (ddx == 0 && ddy == 0) continue;
                            if (updateX + dx < -ddx || updateX + dx >= colDimension - ddx) continue;
                            tmpIdx = (updateY + dy + ddy) * colDimension + updateX + dx + ddx;
                            neighboringMines[tmpIdx]--;
                            unexploredNeighbors[tmpIdx]--;
                            updateNMQueue[updateNMEndIdx++] = tmpIdx;
                            updateUNQueue[updateUNEndIdx++] = tmpIdx;
                        }
                    }
                }
            }
        }
        NM = updateNMStartIdx < updateNMEndIdx;
    }
    
    //printTensor(neighboringMines, rowDimension, colDimension, "neighboringMines");
    //printTensor(unexploredNeighbors, rowDimension, colDimension, "unexploredNeighbors");
    //printTensor(wasSafe, rowDimension, colDimension, "wasSafe");
    //
    //printTensor2(safeQueue + safeQStartIdx, safeQEndIdx - safeQStartIdx, "safeQueue");
    //printTensor2(updateNMQueue + updateNMStartIdx, updateNMEndIdx - updateNMStartIdx, "updateNMQueue");
    //printTensor2(updateUNQueue + updateUNStartIdx, updateUNEndIdx - updateUNStartIdx, "updateUNQueue");
    
    if (safeQStartIdx < safeQEndIdx) {
        pIdx = safeQueue[safeQStartIdx++];
        agentY = pIdx / colDimension;
        agentX = pIdx - agentY * colDimension;
        return {UNCOVER, agentX, agentY};
    }
    
    return {LEAVE,-1,-1};
}