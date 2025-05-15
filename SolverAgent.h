#ifndef SOLVERAGENT_H
#define SOLVERAGENT_H
#include <queue>
#include <vector>
#include "Generator.h"
#include "GeneticAlgorithms.h"

/* * SolverAgent.h
 *
 *  this class handles the solving of the maze, using a solver agent
 *  for now just using A* algo, maybe add more later if required
 *
 */

//basic data struct to order queue
struct Cell {
    int cellID;
    float f_score;
};

//This compares f score values to order the priority queue
struct CompareFScore {
    bool operator() (const Cell& lhs, const Cell& rhs) const {
        return lhs.f_score > rhs.f_score; //order by smallest f
    }
};

class SolverAgent {
public:
    explicit SolverAgent(Maze& maze);
    [[nodiscard]] float calculateHeuristic(int x, int y) const; //manhattan distance
    void solve();

    void setStartPosition(const int x, const int y) {
        startX = x;
        startY = y;
    }
    void setGoalPosition(const int x, const int y) {
        goalX = x;
        goalY = y;
    }

    void rebuild(Maze& maze);

    void printSolution() const;
    void reset();

    [[nodiscard]] const std::vector<int> &getSolution() const {
        return solution;
    }
    [[nodiscard]] const std::vector<int> &getPath() const {
        return path;
    }
    [[nodiscard]] const std::vector<int> &getParents() const {
        return parents;
    }
    [[nodiscard]] const std::vector<bool> &getClosedSet() const {
        return closedSet;
    }

    void loadGenes(const std::string& genesFile);
    void solveGenetic();




private:
    Maze* maze;
    //starting position
    int startX{0};
    int startY{0};
    //goal position
    int goalX{0};
    int goalY{0};

    std::vector<float> fScore; //F = G + H
    std::vector<float> gScore; //number of steps to get to this cell


    std::priority_queue<Cell, std::vector<Cell>, CompareFScore> open_set; //this is the frontier, i.e., leading edge of search
    std::vector<bool> closedSet; //track visited

    //list cell id's for a solution path when completed
    std::vector<int> solution; //this is just the cells for the solution in correct order
    std::vector<int> path; //this is the full step-by-step search path
    std::vector<int> parents; //step back through this to find the solution path

    // direction arrays
    //   0 = Up    (north)
    //   1 = Right (east)
    //   2 = Down  (south)
    //   3 = Left  (west)
    static constexpr int dx[4] = {  0, +1,  0, -1 }; //index these to move left, right, etc.
    static constexpr int dy[4] = { -1,  0, +1,  0 };

    //add mask for walls here, need a better way to store this data across classes
    static constexpr uint8_t WALL_N = 1 << 0;
    static constexpr uint8_t WALL_S = 1 << 1;
    static constexpr uint8_t WALL_E = 1 << 2;
    static constexpr uint8_t WALL_W = 1 << 3;

    //genetic solver stuff
    std::vector<float> genes; //this is the chromosome, i.e., the weights for the policy


};



#endif //SOLVERAGENT_H
