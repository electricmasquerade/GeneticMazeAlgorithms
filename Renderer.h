
#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Generator.h"


class Renderer {
public:
    explicit Renderer(sf::RenderWindow& window, const float thickness): window(window),
                                                                        thickness(thickness) {
        // Initialize the animated maze with the same dimensions as the original maze
        cells.setPrimitiveType(sf::PrimitiveType::Triangles);
        walls.setPrimitiveType(sf::PrimitiveType::Triangles);
    }

    ~Renderer() = default;

    void renderMaze(const Maze& maze);
    void startAnimation(const Maze& maze, const std::vector<Movement>& steps);
    void updateGenAnim(float dt);
    void drawAnim();
    void setFramerateLimit(float framerate) {this->framerate = framerate; timePerFrame = 1.0f / framerate;}
    void buildVertexArrays(const Maze& maze);
    void addQuad(sf::VertexArray &array, float x, float y, int width, int height, uint8_t color);

    [[nodiscard]] bool getAnimationFinished() const {return currentStep >= animatedSteps.size();}
    [[nodiscard]] size_t getAnimationStep() const {return currentStep;}
    [[nodiscard]] size_t getTotalSteps() const {return animatedSteps.size();}
    void setAnimatedMaze(const Maze& maze) {animatedMaze = maze;}
    [[nodiscard]] const Maze& getAnimatedMaze() const {return animatedMaze;}
    [[nodiscard]] const std::vector<Movement>& getAnimatedSteps() const {return animatedSteps;}
    //functions for handling solver animation
    void highlightSolution(const Maze& maze, const std::vector<int>& solution);
    void setDirty() {
        dirty = true;
    }

private:
    sf::RenderWindow& window;
    Maze animatedMaze;
    std::vector<Movement> animatedSteps;
    float thickness = 2;
    sf::VertexArray cells;
    sf::VertexArray walls;

    int currentStep = 0;
    float accumulator = 0;
    float framerate = 60;
    float timePerFrame = 1.0f / framerate;

    static constexpr uint8_t WALL_N = 1 << 0;
    static constexpr uint8_t WALL_S = 1 << 1;
    static constexpr uint8_t WALL_E = 1 << 2;
    static constexpr uint8_t WALL_W = 1 << 3;

    bool dirty = false; //set to true when the maze is changed, so we can rebuild the vertex arrays


};



#endif //RENDERER_H
