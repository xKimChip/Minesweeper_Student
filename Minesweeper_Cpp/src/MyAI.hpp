// ======================================================================
// FILE:        MyAI.hpp
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

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalisMine, int _agentX, int _agentY );
    ~MyAI();
    
    Action getAction ( int number ) override;
    uint32_t pIdx;
    int8_t *neighboringMines, *unexploredNeighbors, *wasSafe;
    uint32_t *safeQueue, safeQStartIdx, safeQEndIdx;
    uint32_t *updateNMQueue, updateNMStartIdx, updateNMEndIdx;
    uint32_t *updateUNQueue, updateUNStartIdx, updateUNEndIdx;
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP