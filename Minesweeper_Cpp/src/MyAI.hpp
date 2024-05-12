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
#include <utility>

using namespace std;

struct MRVCell {
    int x, y;
    int remainingValues; // Potential remaining values for MRV heuristic

    // Constructor for MRVCell
    MRVCell(int _x, int _y, int _remainingValues) : x(_x), y(_y), remainingValues(_remainingValues) {}

    // Comparator for priority queue (we need the smallest values first)
    bool operator<(const MRVCell& other) const {
        return remainingValues > other.remainingValues; // reverse to make it a min-heap
    }
};

class MyAI : public Agent {
public:
    MyAI(int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY);
    Action getAction(int number) override;

private:
    vector<vector<int>> board; // Stores the state of each cell on the board
    set<pair<int, int>> mines; // Keeps track of flagged mine locations
    set<pair<int, int>> safeSpots; // Stores known safe locations
    priority_queue<MRVCell> mrvQueue; // Priority queue for MRV heuristic
    vector<vector<int>> degreeMap; // Map to keep track of the degree heuristic
    
    int rowDimension, colDimension; // Dimensions of the board
    int totalMines; // Total number of mines
    int agentX, agentY; // Current position of the agent
    
    void printQueue();
    void printBoard();
    int calculateMRV(int x, int y); // Calculates the MRV heuristic for a cell
    int calculateDegree(int x, int y); // Calculates the degree heuristic for a cell
    void updateSurroundings(int x, int y, int number); // Updates board based on the number from getAction
    pair<int, int> selectNextMove(); // Chooses the next move based on heuristics
    vector<pair<int, int>> findNeighbors(int x, int y); // Utility to find neighboring cells
    void initializeBoard(); // Additional method to set up the board at the start of the game
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
