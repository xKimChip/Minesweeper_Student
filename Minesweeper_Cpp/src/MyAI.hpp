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
#include <queue>

using namespace std;

class MyAI : public Agent
{
public:
    struct Cell{
        int x, y;
        int num;
        bool operator<(const Cell& other) const {
            return num < other.num;
        }
    };

    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );
    ~MyAI();

    Action getAction ( int number ) override;

    vector<vector<int>> board; // Stores the state of each cell on the board
    bool flagOne = false;
    int counter = 24;
    
    vector<pair<int, int>> BombT;

    priority_queue<Cell> proc;
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
