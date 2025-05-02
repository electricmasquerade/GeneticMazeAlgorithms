
#ifndef GENERATOR_H
#define GENERATOR_H
#include <random>
#include <vector>

//This code handles generating the maze, using depth first or whatever else I decide later
// Create an enum of movement commands, good for GA. Corresponds with the arrays above
enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

struct Maze {
    int width, height;
    std::vector<uint8_t> cells;
    std::vector<bool> visited;
};

struct Movement {
    int x, y;
    Direction direction;
};

class Generator {
public:
    Generator(int width, int height);
    ~Generator() = default;

    void generateMaze();
    void updateStep(Movement movement);
    static void removeWall(Maze& maze, int x, int y, int direction);

    [[nodiscard]] const Maze& getMaze() const{return maze;}
    [[nodiscard]] const std::vector<Movement>& getMovements() const{return movements;}

    void printMaze() const; //probably useless but leaving it here for testing

    void reset();

private:
    std::mt19937 rng;
    Maze maze;
    std::vector<Movement> movements; //steps to generate the maze, useful for rendering but not necessary


    //add mask for walls here.
    static constexpr uint8_t WALL_N = 1 << 0;
    static constexpr uint8_t WALL_S = 1 << 1;
    static constexpr uint8_t WALL_E = 1 << 2;
    static constexpr uint8_t WALL_W = 1 << 3;

    // direction arrays
    //   0 = Up    (north)
    //   1 = Right (east)
    //   2 = Down  (south)
    //   3 = Left  (west)
    static constexpr int dx[4] = {  0, +1,  0, -1 }; //index these to move left, right, etc.
    static constexpr int dy[4] = { -1,  0, +1,  0 };



};



#endif //GENERATOR_H
