
#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Generator.h"


class Renderer {
public:
    explicit Renderer(sf::RenderWindow& window): window(window) {
        // Initialize the animated maze with the same dimensions as the original maze
    }
    ~Renderer() = default;

    void renderMaze(const Maze& maze) const;
    void startAnimation(const Maze& maze, const std::vector<Movement>& steps);
    void updateAnim(float dt);
    void drawAnim();
    void setFramerateLimit(float framerate) {this->framerate = framerate; timePerFrame = 1.0f / framerate;}
    void buildVertexArrays(const Maze& maze);

    [[nodiscard]] bool getAnimationFinished() const {return currentStep >= animatedSteps.size();}
    [[nodiscard]] size_t getAnimationStep() const {return currentStep;}
    [[nodiscard]] size_t getTotalSteps() const {return animatedSteps.size();}

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

};



#endif //RENDERER_H
