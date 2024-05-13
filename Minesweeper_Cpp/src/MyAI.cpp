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

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines = _totalMines;
    agentX = _agentX;
    agentY = _agentY;
    
    // vector<vector<int>> board; set every cell to -1
    board.resize(rowDimension);
    for (int i = 0; i < rowDimension; i++) {
        board[i].resize(colDimension, -2);
    }
};

MyAI::~MyAI ()
{
};

Agent::Action MyAI::getAction( int number )
{
    board[agentX][agentY] = number;
    // debug board
    // for (int i = 0; i < rowDimension; i++) {
    //     for (int j = 0; j < colDimension; j++) {
    //         cout << board[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    if (number == 0 ) { 
        int px, py;
        for (int y = -1; y < 2; y++) {
            py = agentY + y;
            for (int x = -1; x < 2; x++) {
                px = agentX + x;
                if (px >= 0 && px < rowDimension && py >= 0 && py < colDimension) {
                    if ( x == 0 && y == 0 || board[px][py] > -2 ) {
                        continue;
                    }
                    Cell add = { px, py, -1 };
                    board[px][py] = -1;
                    proc.push(add);
                    flagOne = true;
                }
            }
        }
    }
    else {
        BombT.push_back(make_pair(agentX, agentY));
    }
    
    if (BombT.size() >= 2) {
        

    }

    // get top of stack or if stack is empty, return LEAVE
    // counter--;
    if (proc.empty()) {
        // if (counter > 1) {
        //     for (int i = 0; i < rowDimension; i++) {
        //         for (int j = 0; j < colDimension; j++) {
        //             cout << board[i][j] << " ";
        //         }
        //         cout << endl;
        //     }
        // }
        return {LEAVE,-1,-1};
    }
    
    agentX = proc.top().x;
    agentY = proc.top().y;
    proc.pop();

    return {UNCOVER, agentX, agentY};
}

