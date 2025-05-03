#include <iostream>
#include "Generator.h"
#include <algorithm>


Generator::Generator(const int width, const int height) : rng(std::random_device{}()) {
    //init the maze with the given width and height
    maze.width = width;
    maze.height = height;

    //set all walls to full
    maze.cells.resize(width * height);
    maze.visited.resize(width * height, false);
    reset();


}

void Generator::generateMaze() {
    reset();
    printMaze();
    //removeWall(0,0,1);
    // Recursively update the maze, removing walls with depth-first search.
    // Mutate the maze in place, but record steps for rendering later

    //start in the top left always, goal is bottom right.
    Movement startingMovement = { 0, 0, DOWN };
    updateStep(startingMovement);
    //print the maze after generation
    printMaze();


}

void Generator::updateStep(Movement movement) {
    //handle the recursion and updating of the cell and neighbor
    //find current cell with standard formula
    const int cellNum = movement.y * maze.width + movement.x;
    //check if the cell is already visited
    if (maze.visited[cellNum]) {
        return;
    }
    //mark the cell as visited
    maze.visited[cellNum] = true;
    //add the cell to the movements vector, including direction

    //recording too many movements, remove this
    //movements.push_back(movement);

    //find all unvisited neighbors, shuffle array first
    std::array<int, 4> directions = { UP, RIGHT, DOWN, LEFT};
    std::ranges::shuffle(directions, rng);
    for (int i : directions) {
        //find the neighbor cell
        const int neighborX = movement.x + dx[i];
        const int neighborY = movement.y + dy[i];
        //check if the neighbor is out of bounds
        if (neighborX < 0 || neighborX >= maze.width || neighborY < 0 || neighborY >= maze.height) {
            continue;
        }
        const int neighborCellNum = neighborY * maze.width + neighborX;
        //check if the neighbor is visited
        if (maze.visited[neighborCellNum]) {
            continue;
        }
        //remove the wall between the current cell and the neighbor
        removeWall(this->maze, movement.x, movement.y, i);
        //only add remove wall movements to the list
        movements.push_back({ movement.x, movement.y, static_cast<Direction>(i) });
        //update the step for the neighbor
        Movement neighborMovement = { neighborX, neighborY, static_cast<Direction>(i) };
        //recursively update the neighbor
        updateStep(neighborMovement);
    }

}

//   0 = Up    (north)
//   1 = Right (east)
//   2 = Down  (south)
//   3 = Left  (west)
void Generator::removeWall(Maze& maze,const int x, const int y, const int direction) {
    //find current cell with standard formula
    const int cellNum = y * maze.width + x;

    //init the wall and neighbor wall as 0 for now
    uint8_t wall = 0;
    uint8_t neighborWall = 0;
    //find the bit mask for the given wall and the corresponding wall of the neighbor
    switch (direction) {
        case UP:
            wall = WALL_N;
            neighborWall = WALL_S;
            break;
        case RIGHT:
            wall = WALL_E;
            neighborWall = WALL_W;
            break;
        case DOWN:
            wall = WALL_S;
            neighborWall = WALL_N;
            break;
        case LEFT:
            wall = WALL_W;
            neighborWall = WALL_E;
            break;
        default:
            return;
    }

    //remove the wall in the given direction as long as not border wall
    //check if the wall is a border wall and cell is a border cell



    //remove the wall in the opposite direction for the neighbor
    const int neighborX = x + dx[direction];
    const int neighborY = y + dy[direction];
    //check if the neighbor is out of bounds
    if (neighborX < 0 || neighborX >= maze.width || neighborY < 0 || neighborY >= maze.height) {
        return;
    }
    const int neighborCellNum = neighborY * maze.width + neighborX;
    maze.cells[neighborCellNum] = maze.cells[neighborCellNum] & ~neighborWall;
    maze.cells[cellNum] = maze.cells[cellNum] & ~wall;




}

void Generator::printMaze() const {
    //print the maze to the console, in a square format
    //print top border first
    std::cout << " ";
    for (int x = 0; x < maze.width; ++x) {
        std::cout << ((maze.cells[x] & WALL_N) ? " _  " : "  ");
    }
    std::cout << std::endl;

    for (int y = 0; y < maze.height; ++y) {
        std::cout << "|";
        for (int x = 0; x < maze.width; ++x) {
            int cellNum = y * maze.width + x;
            std::cout << ((maze.cells[cellNum] & WALL_S) ? " _ " : "   ");
            std::cout << ((maze.cells[cellNum] & WALL_E) ? "|" : " ");
        }
        std::cout << std::endl;
    }
}

void Generator::reset() {
    for (int i = 0; i < maze.width * maze.height; ++i) {
        maze.cells[i] = WALL_N | WALL_S | WALL_E | WALL_W;
        maze.visited[i] = false;
    }
}
