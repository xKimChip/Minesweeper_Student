#include "MyAI.hpp"

void printTensor(int8_t *tensor, uint8_t height, uint8_t width, const char *name) {
    printf("%s:\n", name);
    for (uint8_t y = height; y--;) {
        for (uint8_t x = 0; x < width; x++) {
            printf("%d ", tensor[y * width + x]);
        }
        printf("\n");
    }
    printf("\n");
}

void printqueue(uint16_t *queue, uint16_t size, uint8_t width, const char *name) {
    printf("%s: \n", name);
    for (uint16_t i = 0; i < size; i++) {
        printf("(%u %u) ", queue[i] % width, queue[i] / width);
    }
    printf("\n");
}

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines = _totalMines;
    agentY = _agentY;
    agentX = _agentX;
    playerIdx = agentY * colDimension + agentX;
    
    neighboringMines = new int8_t[rowDimension * colDimension]{};
    unexploredNeighbors = new int8_t[rowDimension * colDimension];
    neighboringSets = new int8_t[rowDimension * colDimension]{};
    isKnown = new int8_t[rowDimension * colDimension]{};
    potentialMines = new int8_t[rowDimension * colDimension]{};
    tilesInSet = new int8_t[rowDimension * colDimension]{};
    safeQueue = new uint16_t[rowDimension * colDimension]{};
    updateQueue = new uint16_t[rowDimension * colDimension]{};
    
    for (y = rowDimension; y--;) {
        for (x = colDimension; x--;) {
            switch (((y == 0) || (y == rowDimension - 1)) + ((x == 0) || (x == colDimension - 1))) {
                case 0: unexploredNeighbors[y * colDimension + x] = 8; break;
                case 1: unexploredNeighbors[y * colDimension + x] = 5; break;
                case 2: unexploredNeighbors[y * colDimension + x] = 3; break;
            }
        }
    }
    
    safeQStartIdx = 0;
    safeQEndIdx = 0;
    updateQStartIdx = 0;
    updateQEndIdx = 0;
    invCol = 1.0f / colDimension;
    
    isKnown[playerIdx] |= 0b0001; // dont explore
};

MyAI::~MyAI ()
{
    delete[] neighboringMines;
    delete[] unexploredNeighbors;
    delete[] isKnown;
    delete[] potentialMines;
    delete[] tilesInSet;
    delete[] safeQueue;
    delete[] updateQueue;
};

Agent::Action MyAI::getAction( int number )
{
    // printf("player (%u %u) number %u\n", agentX, agentY, number);
    neighboringMines[playerIdx] += number;
    updateQueue[updateQEndIdx++] = playerIdx;
    
    for (dy = -1; dy <= 1; dy++) {
        if (agentY == 0 && dy == -1) continue;
        if (agentY == rowDimension - 1 && dy == 1) continue;
        for (dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            if (agentX == 0 && dx == -1) continue;
            if (agentX == colDimension - 1 && dx == 1) continue;
            tempIdx = (agentY + dy) * colDimension + agentX + dx;
            // printf("update cuz player (%u %u)\n", agentX + dx, agentY + dy);
            // update neighbor set probabilities
            unexploredNeighbors[tempIdx]--;
            if (isKnown[tempIdx] & 0b1000) continue; // in update queue
            updateQueue[updateQEndIdx++] = tempIdx;
            isKnown[tempIdx] |= 0b1100; // in update queue and only unexploredNeighbors is updated
        }
    }
    
    do {
        do {
            updateIdx = updateQueue[updateQStartIdx++];
            y = updateIdx * invCol;
            x = updateIdx - y * colDimension;
            isKnown[updateIdx] &= 0b0111; // remove from update queue
            // printf("update (%u %u)\n", x, y);
            // dont go in if only unexploredNeighbors is updated, if 0 mines, all neighbors are safe
            if ((isKnown[updateIdx] & 0b0100) == 0 && neighboringMines[updateIdx] == 0) {
                // printf("state 1 (%u %u)\n", x, y);
                for (dy = -1; dy <= 1; dy++) {
                    if (y == 0 && dy == -1) continue;
                    if (y == rowDimension - 1 && dy == 1) continue;
                    for (dx = -1; dx <= 1; dx++) {
                        if (dy == 0 && dx == 0) continue;
                        if (x == 0 && dx == -1) continue;
                        if (x == colDimension - 1 && dx == 1) continue;
                        tempIdx = (y + dy) * colDimension + x + dx;
                        if (isKnown[tempIdx] & 0b0001) continue; // dont explore
                        isKnown[tempIdx] |= 0b0001; // dont explore
                        safeQueue[safeQEndIdx++] = tempIdx;
                        // printf("safe (%u %u)\n", x + dx, y + dy);
                    }
                }
            // if the number of mines is equal to the number of unexplored neighbors, all neighbors are mines
            } else if (neighboringMines[updateIdx] == unexploredNeighbors[updateIdx]) {
                // printf("state 2 (%u %u)\n", x, y);
                isKnown[updateIdx] &= 0b1011; // remove from only unexploredNeighbors updated
                for (dy = -1; dy <= 1; dy++) {
                    if (y == 0 && dy == -1) continue;
                    if (y == rowDimension - 1 && dy == 1) continue;
                    for (dx = -1; dx <= 1; dx++) {
                        if (dy == 0 && dx == 0) continue;
                        if (x == 0 && dx == -1) continue;
                        if (x == colDimension - 1 && dx == 1) continue;
                        tempIdx = (y + dy) * colDimension + x + dx;
                        if (isKnown[tempIdx] & 0b0001) continue; // dont explore
                        isKnown[tempIdx] |= 0b0011; // mine and dont explore
                        totalMines--;
                        // printf("mine (%u %u)\n", x + dx, y + dy);
                        
                        for (dyy = -1; dyy <= 1; dyy++) {
                            if (y + dy == 0 && dyy == -1) continue;
                            if (y + dy == rowDimension - 1 && dyy == 1) continue;
                            for (dxx = -1; dxx <= 1; dxx++) {
                                if (dyy == 0 && dxx == 0) continue;
                                if (x + dx == 0 && dxx == -1) continue;
                                if (x + dx == colDimension - 1 && dxx == 1) continue;
                                updateIdx = (y + dy + dyy) * colDimension + x + dx + dxx;
                                unexploredNeighbors[updateIdx]--;
                                neighboringMines[updateIdx]--;
                                // printf("update cuz mine (%u %u)\n", x + dx + dxx, y + dy + dyy);
                                isKnown[updateIdx] &= 0b1011; // remove from only unexploredNeighbors updated
                                if (isKnown[updateIdx] & 0b1000) continue; // in update queue
                                updateQueue[updateQEndIdx++] = updateIdx;
                                isKnown[updateIdx] |= 0b1000; // in update queue;
                            }
                        }
                    }
                }
            } 
             else isKnown[updateIdx] &= 0b1011; // remove from only unexploredNeighbors updated
        } while (updateQStartIdx < updateQEndIdx);
        updateQStartIdx = 0;
        updateQEndIdx = 0;
        
        // printTensor(neighboringMines, rowDimension, colDimension, "neighboringMines");
        // printTensor(unexploredNeighbors, rowDimension, colDimension, "unexploredNeighbors");
        // printTensor(isKnown, rowDimension, colDimension, "isKnown");
        // printTensor(potentialMines, rowDimension, colDimension, "potentialMines");
        // printTensor(tilesInSet, rowDimension, colDimension, "tilesInSet");
        // printqueue(safeQueue + safeQStartIdx, safeQEndIdx - safeQStartIdx, colDimension, "safeQueue");
        
        if (safeQStartIdx < safeQEndIdx) {
            playerIdx = safeQueue[safeQStartIdx++];
            agentY = playerIdx * invCol;
            agentX = playerIdx - agentY * colDimension;
            return {UNCOVER, agentX, agentY};
        }
        
        // using sets now for major logic
        // printf("sets\n");
        for (y = rowDimension; y--;) {
            for (x = colDimension; x--;) {
                updateIdx = y * colDimension + x;
                if (neighboringMines[updateIdx] <= 0) continue;
                // for (dy = 2; dy--;) {
                for (dy = 3; dy--;) {
                    // printf("dy %u\n", dy);
                    if (y + dy > rowDimension - 1) continue;
                    // for (dx = -1; dx <= 1; dx++) {
                    for (dx = -2; dx <= 2; dx++) {
                        // printf("dx %d\n", dx);
                        // printf("updateIdx (%u %u)\n", x, y);
                        if (dy == 0 && dx == 0) break; // continue no more
                        if (x + dx < 0) continue;
                        if (x + dx > colDimension - 1) continue;
                        tempIdx = (y + dy) * colDimension + x + dx;
                        if (neighboringMines[tempIdx] <= 0) continue;
                        // found a pair that we can use set logic on, determine which has more mines or equal
                        uint16_t largerIdx = neighboringMines[updateIdx] > neighboringMines[tempIdx] ? updateIdx : tempIdx;
                        uint16_t smallerIdx = largerIdx == updateIdx ? tempIdx : updateIdx;
                        uint8_t minesInLarger = neighboringMines[largerIdx] - neighboringMines[smallerIdx];
                        uint8_t largerY = largerIdx * invCol;
                        uint8_t largerX = largerIdx - largerY * colDimension;
                        uint8_t smallerY = smallerIdx * invCol;
                        uint8_t smallerX = smallerIdx - smallerY * colDimension;
                        // printf("pair (%u %u) (%u %u)\n", largerX, largerY, smallerX, smallerY);
                        
                        uint16_t tmpIdx2;
                        uint8_t setAIdx = 0;
                        for (dyy = -1; dyy <= 1; dyy++) {
                            if (largerY == 0 && dyy == -1) continue;
                            if (largerY == rowDimension - 1 && dyy == 1) continue;
                            for (dxx = -1; dxx <= 1; dxx++) {
                                if (dyy == 0 && dxx == 0) continue;
                                if (largerX == 0 && dxx == -1) continue;
                                if (largerX == colDimension - 1 && dxx == 1) continue;
                                tmpIdx2 = (largerY + dyy) * colDimension + largerX + dxx;
                                if (isKnown[tmpIdx2] & 0b0001) continue; // dont explore
                                setA[setAIdx++] = tmpIdx2;
                            }
                        }
                        
                        uint8_t setBIdx = 0;
                        for (dyy = -1; dyy <= 1; dyy++) {
                            if (smallerY == 0 && dyy == -1) continue;
                            if (smallerY == rowDimension - 1 && dyy == 1) continue;
                            for (dxx = -1; dxx <= 1; dxx++) {
                                if (dyy == 0 && dxx == 0) continue;
                                if (smallerX == 0 && dxx == -1) continue;
                                if (smallerX == colDimension - 1 && dxx == 1) continue;
                                tmpIdx2 = (smallerY + dyy) * colDimension + smallerX + dxx;
                                if (isKnown[tmpIdx2] & 0b0001) continue; // dont explore
                                setB[setBIdx++] = tmpIdx2;
                            }
                        }
                        
                        // printf("setAOnly: ");
                        uint8_t sharedIdx = 0;
                        uint8_t setAOnlyIdx = 0;
                        for (uint8_t i = 0; i < setAIdx; i++) {
                            bool found = false;
                            for (uint8_t j = 0; j < setBIdx; j++) {
                                if (setA[i] == setB[j]) {
                                    found = true;
                                    break;
                                }
                            }
                            if (found) {
                                shared[sharedIdx++] = setA[i];
                            } else {
                                setAOnly[setAOnlyIdx++] = setA[i];
                                // printf("(%u %u) ", setA[i] % colDimension, setA[i] / colDimension);
                            }
                        }
                        // printf("\n");
                        
                        // printf("setBOnly: ");
                        uint8_t setBOnlyIdx = 0;
                        for (uint8_t i = 0; i < setBIdx; i++) {
                            bool found = false;
                            for (uint8_t j = 0; j < setAIdx; j++) {
                                if (setB[i] == setA[j]) {
                                    found = true;
                                    break;
                                }
                            }
                            if (found) continue;
                            setBOnly[setBOnlyIdx++] = setB[i];
                            // printf("(%u %u) ", setB[i] % colDimension, setB[i] / colDimension);
                        }
                        // printf("\n");
                        
                        // printf("shared: ");
                        for (uint8_t i = 0; i < sharedIdx; i++) {
                            // printf("(%u %u) ", shared[i] % colDimension, shared[i] / colDimension);
                        }
                        // printf("\n");
                        
                        // if num mines in larger set only is equal to num unexplored in larger set only, all in larger set only are mines, all in smaller set only are safe
                        if (minesInLarger == setAOnlyIdx) {
                            for (uint8_t i = 0; i < setAOnlyIdx; i++) {
                                playerIdx = setAOnly[i];
                                agentY = playerIdx * invCol;
                                agentX = playerIdx - agentY * colDimension;
                                if (isKnown[playerIdx] & 0b0001) continue; // dont explore
                                isKnown[playerIdx] |= 0b0011; // mine and dont explore
                                totalMines--;
                                // printf("mine sets 3 (%u %u)\n", agentX, agentY);
                                
                                for (dyy = -1; dyy <= 1; dyy++) {
                                    if (agentY == 0 && dyy == -1) continue;
                                    if (agentY == rowDimension - 1 && dyy == 1) continue;
                                    for (dxx = -1; dxx <= 1; dxx++) {
                                        if (dyy == 0 && dxx == 0) continue;
                                        if (agentX == 0 && dxx == -1) continue;
                                        if (agentX == colDimension - 1 && dxx == 1) continue;
                                        tempIdx = (agentY + dyy) * colDimension + agentX + dxx;
                                        unexploredNeighbors[tempIdx]--;
                                        neighboringMines[tempIdx]--;
                                        // printf("update cuz mine sets (%u %u)\n", agentX + dxx, agentY + dyy);
                                        isKnown[tempIdx] &= 0b1011; // remove from only unexploredNeighbors updated
                                        if (isKnown[tempIdx] & 0b1000) continue; // in update queue
                                        updateQueue[updateQEndIdx++] = tempIdx;
                                        isKnown[tempIdx] |= 0b1000; // in update queue;
                                    }
                                }
                            }
                            for (uint8_t i = 0; i < setBOnlyIdx; i++) {
                                playerIdx = setBOnly[i];
                                agentY = playerIdx * invCol;
                                agentX = playerIdx - agentY * colDimension;
                                if (isKnown[playerIdx] & 0b0001) continue; // dont explore
                                isKnown[playerIdx] |= 0b0001; // dont explore
                                safeQueue[safeQEndIdx++] = playerIdx;
                                // printf("safe sets 3 (%u %u)\n", agentX, agentY);
                            }
                            // totalMines -= neighboringMines[smallerIdx];
                            // printf("there are 0 bombs in BOnly\n");
                            // printf("there are %u bombs in shared\n", neighboringMines[smallerIdx]);
                            // for each tile in shared, potential mines is neighboring mines of b, tiles in set is size of shared
                            for (uint8_t i = 0; i < sharedIdx; i++) {
                                // if (potentialMines[shared[i]] != 0) printf("potentialMines is not 0\n");
                                potentialMines[shared[i]] = neighboringMines[smallerIdx];
                                tilesInSet[shared[i]] = sharedIdx;
                            }
                        } else if (minesInLarger == 0 && 0 == setBOnlyIdx) {
                            // if both neighbors have equal mines and one set only had no unexplored neighbors, all in the other set are safe
                            for (uint8_t i = 0; i < setAOnlyIdx; i++) {
                                playerIdx = setAOnly[i];
                                agentY = playerIdx * invCol;
                                agentX = playerIdx - agentY * colDimension;
                                if (isKnown[playerIdx] & 0b0001) continue; // dont explore
                                isKnown[playerIdx] |= 0b0001; // dont explore
                                safeQueue[safeQEndIdx++] = playerIdx;
                                // printf("safe sets 4 (%u %u)\n", agentX, agentY);
                            }
                            // totalMines -= neighboringMines[largerIdx];
                            // printf("there are 0 bombs in AOnly\n");
                            // printf("there are %u bombs in shared\n", neighboringMines[largerIdx]);
                            // for each tile in shared, potential mines is neighboring mines of a, tiles in set is size of shared
                            for (uint8_t i = 0; i < sharedIdx; i++) {
                                // if (potentialMines[shared[i]] != 0) printf("potentialMines is not 0\n");
                                potentialMines[shared[i]] = neighboringMines[largerIdx];
                                tilesInSet[shared[i]] = sharedIdx;
                            }
                        }
                    }
                }
            }
        }
        
        // printTensor(potentialMines, rowDimension, colDimension, "potentialMines");
        // printTensor(tilesInSet, rowDimension, colDimension, "tilesInSet");
    } while (updateQStartIdx < updateQEndIdx || safeQStartIdx < safeQEndIdx);
    safeQStartIdx = 0;
    safeQEndIdx = 0;
    
    // as a last resort, a random unexplored cell
    // printf("last resort\n");
    for (y = rowDimension; y--;) {
        for (x = colDimension; x--;) {
            tempIdx = y * colDimension + x;
            if (isKnown[tempIdx] & 0b0001) continue; // dont explore
            updateQueue[updateQEndIdx++] = tempIdx;
        }
    }
    if (updateQEndIdx == 0) return {LEAVE, -1, -1}; // no unexplored cells
    
    float tmpTotalMines = totalMines;
    for (updateQStartIdx = 0; updateQStartIdx < updateQEndIdx; updateQStartIdx++) {
        if (potentialMines[updateQueue[updateQStartIdx]] != 0) {
            tmpTotalMines -= (float)potentialMines[updateQueue[updateQStartIdx]] / tilesInSet[updateQueue[updateQStartIdx]];
        } else {
            // write tile to safe queue to show it it part of the unexplored set
            safeQueue[safeQEndIdx++] = updateQueue[updateQStartIdx];
        }
    }
    // printf("tmpTotalMines %f\n", tmpTotalMines);
    // for the unexplored set, set the potential mines to the temp total mines / size of unexplored set
    for (uint16_t i = 0; i < safeQEndIdx; i++) {
        potentialMines[safeQueue[i]] = tmpTotalMines;
        tilesInSet[safeQueue[i]] = safeQEndIdx;
    }
    safeQStartIdx = 0;
    safeQEndIdx = 0;
    // printTensor(potentialMines, rowDimension, colDimension, "potentialMines");
    // printTensor(tilesInSet, rowDimension, colDimension, "tilesInSet");
    playerIdx = updateQueue[0];
    float lowestProb = (float)potentialMines[playerIdx] / tilesInSet[playerIdx];
    // uint16_t itr = 1;
    for (updateQStartIdx = 1; updateQStartIdx < updateQEndIdx; updateQStartIdx++) {
        float prob = (float)potentialMines[updateQueue[updateQStartIdx]] / tilesInSet[updateQueue[updateQStartIdx]];
        // printf("prob %f at (%u %u)\n", prob, updateQueue[updateQStartIdx] % colDimension, updateQueue[updateQStartIdx] / colDimension);
        if (prob < lowestProb) {
            lowestProb = prob;
            playerIdx = updateQueue[updateQStartIdx];
        }
            // itr = 1;
        // } else if (prob == lowestProb) {
        //     itr++;
            
        //     float tmp = (float)rand() / RAND_MAX;
        //     if (tmp < 1.0f / itr) {
        //         playerIdx = updateQueue[updateQStartIdx];
        //     }
        // }
    }
    
    // printTensor(neighboringMines, rowDimension, colDimension, "neighboringMines");
    // printTensor(unexploredNeighbors, rowDimension, colDimension, "unexploredNeighbors");
    // printTensor(isKnown, rowDimension, colDimension, "isKnown");
    // printTensor(potentialMines, rowDimension, colDimension, "potentialMines");
    // printTensor(tilesInSet, rowDimension, colDimension, "tilesInSet");
    // printqueue(safeQueue + safeQStartIdx, safeQEndIdx - safeQStartIdx, colDimension, "safeQueue");
        
    //printTensor(neighboringMines, rowDimension, colDimension, "neighboringMines");
    //    printTensor(unexploredNeighbors, rowDimension, colDimension, "unexploredNeighbors");
    //    printTensor(isKnown, rowDimension, colDimension, "isKnown");
    //    printTensor(potentialMines, rowDimension, colDimension, "potentialMines");
    //    printTensor(tilesInSet, rowDimension, colDimension, "tilesInSet");
        // printqueue(safeQueue + safeQStartIdx, safeQEndIdx - safeQStartIdx, colDimension, "safeQueue");
        
    
    // printf("random unexplored\n");
    // playerIdx = updateQueue[std::chrono::high_resolution_clock::now().time_since_epoch().count() % updateQEndIdx];
    updateQStartIdx = 0;
    updateQEndIdx = 0;
    agentY = playerIdx * invCol;
    agentX = playerIdx - agentY * colDimension;
    isKnown[playerIdx] |= 0b0001; // dont explore
    // if (totalMines != 0)
    //     printf("last resort has totalMines %u\n", totalMines);
    return {UNCOVER, agentX, agentY};
}

//Counting total number of mines in all sets, if the total number of mines