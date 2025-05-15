
#include "SolverAgent.h"
#include <algorithm>
#include <fstream>
#include <iostream>

SolverAgent::SolverAgent(Maze& maze) {
    this->maze = &maze;
    rebuild(maze);

}

float SolverAgent::calculateHeuristic(const int x, const int y) const {
    // Calculate the heuristic value for the A* algorithm
    // Using Manhattan distance as the heuristic
    // This is the sum of the absolute differences in the x and y coordinates
    // between the current position and the goal position

    return abs(x - goalX) + abs(y - goalY);
}

void SolverAgent::solve() {
    //reset/clear all data
    reset();
    // Implement the A* algorithm to solve the maze
    // Initialize the open set with the starting position
    const int startCellID = startY * maze->width + startX;
    const int goalCellID = goalY * maze->width + goalX;

    gScore[startCellID] = 0;
    fScore[startCellID] = calculateHeuristic(startX, startY);

    open_set.push({startCellID, fScore[startCellID]});
    static constexpr int wallMasks[4] = {
        WALL_N,
        WALL_E,
        WALL_S,
        WALL_W
    };
    // Main loop of the A* algorithm
    while (!open_set.empty()) {
        auto current = open_set.top();
        open_set.pop();
        const int currentCellID = current.cellID;
        const int currentX = currentCellID % maze->width;
        const int currentY = currentCellID / maze->width;

        //check if already visited somehow
        if (closedSet[currentCellID]) {
            continue;
        }

        closedSet[currentCellID] = true;
        path.push_back(currentCellID);

        // Check if we reached the goal
        if (currentCellID == goalCellID) {
            // Reconstruct the path from the goal to the start
            int cell = currentCellID;
            while (cell != startCellID) {
                solution.push_back(cell);
                cell = parents[cell];
            }
            solution.push_back(startCellID); // Add the start cell to the solution
            std::ranges::reverse(solution);
            return;
        }

        //check neighbors
        for (int direction = 0; direction < 4; direction++) {
            auto neighborX = currentX + dx[direction];
            auto neighborY = currentY + dy[direction];

            //check if the neighbor is out of bounds
            if (neighborX < 0 || neighborX >= maze->width || neighborY < 0 || neighborY >= maze->height) {
                continue;
            }
            const int neighborCellID = neighborY * maze->width + neighborX;

            //check for wall between neighbor and current with wall mask
            if (maze->cells[currentCellID] & wallMasks[direction]) {
                continue;
            }

            //check if the neighbor is already in the closed set
            if (closedSet[neighborCellID]) {
                continue;
            }
            // Calculate the tentative gScore for the neighbor
            float tentative_gScore = gScore[currentCellID] + 1; //assuming uniform cost for each step
            // If the neighbor is not in the open set or the new gScore is better
            if (tentative_gScore < gScore[neighborCellID]) {
                // This path to the neighbor is better than any previous one
                parents[neighborCellID] = currentCellID;
                gScore[neighborCellID] = tentative_gScore;
                fScore[neighborCellID] = gScore[neighborCellID] + calculateHeuristic(neighborX, neighborY);

                open_set.push({neighborCellID, fScore[neighborCellID]});
            }
        }
    }
    }

void SolverAgent::rebuild(Maze &maze) {
    //re-init all data when maze is generated at a new size
    this->maze = &maze;
    // Initialize the closed set with the size of the maze
    closedSet.resize(maze.width * maze.height, false);
    // Initialize the fScore and gScore vectors with the size of the maze
    // Set the initial fScore and gScore to infinity, since we want smallest distance/score
    fScore.resize(maze.width * maze.height, std::numeric_limits<float>::max());
    gScore.resize(maze.width * maze.height, std::numeric_limits<float>::max());

    //init the parent cell vector
    parents.resize(maze.width * maze.height, -1);

    // Set the starting position and goal position
    startX = 0;
    startY = 0;
    goalX = static_cast<int>(maze.width) - 1;
    goalY = static_cast<int>(maze.height) - 1;

}

void SolverAgent::printSolution() const {
    // Print the solution path
    std::cout << "Solution Path: ";
    for (const auto& cell : solution) {
        std::cout << "(" << cell % maze->width << ", " << cell / maze->width << ") ";
    }
    std::cout << std::endl;
}

void SolverAgent::reset() {
    // Reset solver state for a fresh run
    std::ranges::fill(closedSet, false);
    std::ranges::fill(gScore,   std::numeric_limits<float>::max());
    std::ranges::fill(fScore,   std::numeric_limits<float>::max());
    std::ranges::fill(parents,  -1);

    // clear out any old nodes still in the priority queue
    while (!open_set.empty()) open_set.pop();

    // now clear your output paths
    path.clear();
    solution.clear();
}

void SolverAgent::loadGenes(const std::string& genesFile) {
    //load genes from binary file
    //read the genes from the file

    //check if file is valid
    std::ifstream file(genesFile, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error opening file for reading: " << genesFile << std::endl;
        return;
    }
    const auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    const size_t expectedSize = GeneticAlgorithms::getNumGenes() * sizeof(float);
    if (static_cast<size_t>(fileSize) != expectedSize) {
        std::cerr << "Error: file size does not match expected size" << std::endl;
        return;
    }

    genes.resize(GeneticAlgorithms::getNumGenes());
    file.read(reinterpret_cast<char*>(genes.data()), expectedSize);
    for (const float num : genes) {
        std::cout<< num << " ";
    }
    std::cout << std::endl;



}

void SolverAgent::solveGenetic() {
    //reset/clear all data
    reset();
    // Implement the A* algorithm to solve the maze
    // Initialize the open set with the starting position
    const int startCellID = startY * maze->width + startX;
    const int goalCellID = goalY * maze->width + goalX;


    static constexpr int wallMasks[4] = {
        WALL_N,
        WALL_E,
        WALL_S,
        WALL_W
    };
    //check for genes first
    if (genes.empty()) {
        std::cerr << "Error: no genes loaded" << std::endl;
        return;
    }

    int currentCellID = startCellID;
    path.clear();
    solution.clear();
    path.push_back(currentCellID);

    //solve maze using genetic algorithm
    int feature_size = GeneticAlgorithms::getNumInputs();
    std::vector<float> features{};
    features.resize(feature_size);

    int steps = 0;

    //run until hits goal or max steps
    while (steps < GeneticAlgorithms::getMaxSteps() && currentCellID != goalCellID) {
        //initialize the features with the wall masks and distances and stuff
        int walls = maze->cells[currentCellID];
        for (int direction = 0; direction < 4; ++direction) {
            if (walls & wallMasks[direction]) {
                features[direction] = 1.0f; //wall present
            }
            else {
                features[direction] = 0.0f; //no wall
            }
        }
        int goalX = goalCellID % maze->width;
        int goalY = goalCellID / maze->width;
        features[4] = (goalX - currentCellID % maze->width) / static_cast<float>(maze->width);
        features[5] = (goalY - currentCellID / maze->width) / static_cast<float>(maze->height);
        features[6] = 1.0f; //bias term


        std::vector<float> outputs{};
        const int numOutputs = GeneticAlgorithms::getNumOutputs();
        outputs.resize(numOutputs);

        for (int i = 0; i < numOutputs; ++i) {
            float score = 0;
            for (int j = 0; j < feature_size; ++j) {
                score += features[j] * genes[i * feature_size + j];
            }
            outputs[i] = score;
        }

        // find the direction with the highest score
        int best = 0;
        float bestScore = outputs[0];
        for (int i = 1; i < numOutputs; ++i) {
            if (outputs[i] > bestScore || (outputs[i] == bestScore && std::rand() % 2)) {
                bestScore = outputs[i];
                best = i;
            }
        }
        //check for neighbor out of bounds
        const int neighborX = currentCellID % maze->width + dx[best];
        const int neighborY = currentCellID / maze->width + dy[best];
        if (neighborX < 0 || neighborX >= maze->width || neighborY < 0 || neighborY >= maze->height) {
            //out of bounds, waste step
            steps++;
            continue;
        }
        //if direction is blocked, waste step
        if (maze->cells[currentCellID] & wallMasks[best]) {
            //blocked, waste step
            steps++;
            continue;
        }
        //add to path


        currentCellID = neighborY * maze->width + neighborX; //move to new cell
        path.push_back(currentCellID);

        //increment the step
        steps++;
        std::cout << steps << std::endl;
        //check if we reached the goal
        if (currentCellID == goalCellID) {
            solution = path;
        }

    }
}

