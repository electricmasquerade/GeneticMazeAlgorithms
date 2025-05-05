#ifndef SOLVERAGENT_H
#define SOLVERAGENT_H
#include <queue>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Generator.h"

/* * SolverAgent.h
 *
 *  this class handles the solving of the maze, using a solver agent
 *  for now just using A* algo, maybe add more later if required
 *
 */



class SolverAgent {

private:
    const Maze& maze;
    //starting position
    int startX{0};
    int startY{0};
    //goal position
    size_t goalX{maze.width};
    size_t goalY{maze.height};

    std::priority_queue<std::pair<int, int>> open_set;
    std::priority_queue<std::pair<int, int>> closed_set;


};



#endif //SOLVERAGENT_H
