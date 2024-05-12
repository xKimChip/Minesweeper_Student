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

MyAI::MyAI(int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY) : Agent(),
    rowDimension(_rowDimension), colDimension(_colDimension), totalMines(_totalMines), 
    agentX(_agentX), agentY(_agentY)
{
    board.resize(rowDimension, vector<int>(colDimension, -1)); // -1 for unexplored cells
    // mines and safeSpots do not need reserve calls
    safeSpots.insert({_agentX, _agentY}); // Set the initial known safe spot
    board[_agentY][_agentX] = 0; // This is the starting point with no mines adjacent
    
    printBoard();
}

Agent::Action MyAI::getAction(int number)
{
    cout << "Received number: " << number << " at [" << agentX << ", " << agentY << "]\n";
    board[agentY][agentX] = number;

    updateSurroundings(agentX, agentY, number);
    printBoard(); // Print board after updating surroundings

    pair<int, int> nextMove = selectNextMove();
    printQueue(); // Print MRV queue before selecting next move

    if (nextMove.first == -1 && nextMove.second == -1) {
        cout << "No valid move found, AI will leave.\n";
        return {LEAVE, -1, -1};
    }

    cout << "Next move: [" << nextMove.first << ", " << nextMove.second << "]\n";
    agentX = nextMove.first;
    agentY = nextMove.second;

    return {UNCOVER, agentX, agentY};
}

void MyAI::printBoard() {
    cout << "Board state:\n";
    for (auto &row : board) {
        for (int cell : row) {
            cout << cell << " ";
        }
        cout << "\n";
    }
}

void MyAI::printQueue() {
    cout << "Current MRV Queue state:\n";
    auto tempQueue = mrvQueue;  // Copy the queue to not disturb the original
    while (!tempQueue.empty()) {
        auto cell = tempQueue.top();
        tempQueue.pop();
        cout << "[" << cell.x << ", " << cell.y << "]: MRV = " << cell.remainingValues << "\n";
    }
}

int MyAI::calculateMRV(int x, int y) {
    int mrv = 0; // Start with zero possible mine configurations
    vector<pair<int, int>> neighbors = findNeighbors(x, y);
    
    // Iterate through each neighbor to count the clues and flagged mines
    for (auto &n : neighbors) {
        int nx = n.first, ny = n.second;
        if (board[ny][nx] >= 0) { // Only consider cells that are uncovered and have a clue
            int minesAround = board[ny][nx];
            int flaggedMines = 0;
            int uncoveredNeighbors = 0;

            // Count flagged mines and uncoverable neighbors around the current neighbor
            vector<pair<int, int>> subNeighbors = findNeighbors(nx, ny);
            for (auto &sn : subNeighbors) {
                if (board[sn.second][sn.first] == -2) flaggedMines++; // Cell is flagged as a mine
                if (board[sn.second][sn.first] == -1) uncoveredNeighbors++; // Unexplored cell
            }

            // Calculate possible configurations based on remaining mines to be found around this neighbor
            int remainingMines = minesAround - flaggedMines;
            if (remainingMines > 0 && uncoveredNeighbors > 0) {
                mrv += remainingMines; // The fewer mines left to find, the higher the certainty
            }
        }
    }
    
    return mrv;
}

int MyAI::calculateDegree(int x, int y) {
    int degree = 0;
    vector<pair<int, int>> neighbors = findNeighbors(x, y);

    // Count the number of neighboring cells that are still unexplored
    for (auto &n : neighbors) {
        int nx = n.first, ny = n.second;
        if (board[ny][nx] == -1) { // Unexplored cells contribute to the degree
            degree++;
        }
    }

    return degree;
}


void MyAI::updateSurroundings(int x, int y, int number) {
    vector<pair<int, int>> neighbors = findNeighbors(x, y);
    if (number == 0) {
        for (auto &n : neighbors) {
            int nx = n.first, ny = n.second;
            if (board[ny][nx] == -1) {  // Check if unexplored
                board[ny][nx] = 0;  // Update as safe
                safeSpots.insert({nx, ny});
                mrvQueue.push(MRVCell(nx, ny, 0)); // MRV is 0 since it's safe
                std::cout << "Added to MRV Queue: [" << nx << ", " << ny << "] with MRV 0\n";
            }
        }
    } else {
        int countMines = 0, countUnexplored = 0;
        for (auto &n : neighbors) {
            int nx = n.first, ny = n.second;
            if (board[ny][nx] == -1) ++countUnexplored; // Unexplored cell
            else if (board[ny][nx] == -2) ++countMines;  // Already flagged as mine
        }

        if (number == countUnexplored) {
            for (auto &n : neighbors) {
                int nx = n.first, ny = n.second;
                if (board[ny][nx] == -1) {
                    board[ny][nx] = -2; // Flag as mine
                    mines.insert({nx, ny});
                }
            }
        }

        // Update MRV and Degree for each neighbor
        for (auto &n : neighbors) {
            int nx = n.first, ny = n.second;
            if (board[ny][nx] == -1) { // Only update MRV and Degree for unexplored cells
                int mrv = calculateMRV(nx, ny); // This function needs to be defined to calculate MRV based on potential mine configurations
                mrvQueue.push(MRVCell(nx, ny, mrv));
                degreeMap[ny][nx] = calculateDegree(nx, ny); // Calculate degree heuristic
            }
        }
    }
}


vector<pair<int, int>> MyAI::findNeighbors(int x, int y) {
    vector<pair<int, int>> neighbors;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < colDimension && ny >= 0 && ny < rowDimension) {
                neighbors.push_back({nx, ny});
            }
        }
    }
    return neighbors;
}

pair<int, int> MyAI::selectNextMove() {
    while (!mrvQueue.empty()) {
        MRVCell top = mrvQueue.top();
        mrvQueue.pop();

        int x = top.x, y = top.y;
        cout << "Checking cell from MRV Queue: [" << x << ", " << y << "] with MRV " << top.remainingValues << "\n";

        // Check if this cell is still unexplored
        if (board[y][x] == -1) {  // Unexplored cells are potential candidates
            cout << "Selected cell: [" << x << ", " << y << "] for action.\n";
            return {x, y};
        } else {
            cout << "Skipped cell: [" << x << ", " << y << "] as it is already explored or flagged.\n";
        }
    }

    cout << "No valid move found in the queue, AI will leave.\n";
    // If no valid move is found, return {-1, -1}
    return {-1, -1};
}

