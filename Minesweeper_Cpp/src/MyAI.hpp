#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <chrono>

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalisMine, int _agentX, int _agentY );
    ~MyAI();
    
    Action getAction ( int number ) override;
    uint16_t playerIdx;
    int8_t *neighboringMines, *unexploredNeighbors, *isKnown, *potentialMines, *tilesInSet, *neighboringSets;
    uint16_t *safeQueue, safeQStartIdx, safeQEndIdx;
    uint16_t *updateQueue, updateQStartIdx, updateQEndIdx;
    
    float invCol;
    int8_t dx, dy;
    uint8_t x, y;
    int8_t dxx, dyy;
    uint16_t tempIdx, updateIdx, setA[8], setB[8], setBOnly[8], setAOnly[8], shared[8];
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP