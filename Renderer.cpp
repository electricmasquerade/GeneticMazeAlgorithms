#include "Renderer.h"
#include <SFML/Graphics.hpp>


void Renderer::renderMaze(const Maze &maze) {
    //window.clear(sf::Color::White);

    buildVertexArrays(maze);
    //draw the cells
    window.draw(cells);
    window.draw(walls);
}

void Renderer::startAnimation(const Maze &maze, const std::vector<Movement> &steps) {
    //set the animated maze to the original maze
    animatedMaze.height = maze.height;
    animatedMaze.width = maze.width;
    animatedMaze.cells.resize(animatedMaze.height * animatedMaze.width);
    for (int i = 0; i < animatedMaze.width * animatedMaze.height; ++i) {
        animatedMaze.cells[i] = WALL_N | WALL_S | WALL_E | WALL_W;
    }
    animatedSteps = steps;
    currentStep = 0;
    accumulator = 0;
    buildVertexArrays(animatedMaze);
}

void Renderer::updateGenAnim(const float dt) {
    //handle one step of the animation
    accumulator += dt;
    while (accumulator >= timePerFrame && currentStep < animatedSteps.size()) {
        //get the current step
        Movement movement = animatedSteps[currentStep];
        //remove the wall in the given direction
        Generator::removeWall(animatedMaze, movement.x, movement.y, movement.direction);
        //increment the step
        currentStep++;
        accumulator -= timePerFrame;
    }
}

void Renderer::drawAnim() {
    renderMaze(animatedMaze);
}

void Renderer::buildVertexArrays(const Maze &maze) {
    cells.clear();
    walls.clear();

    //get maze dimensions
    const size_t width = maze.width;
    const size_t height = maze.height;
    if (width <= 0 || height <= 0) {
        return;
    }
    // cells.resize(maze.width * maze.height * 6);
    // walls.resize(maze.width * maze.height * 6 * 4); // 4 walls per cell, 6 vertices per wall
    //get the size of the window
    const sf::Vector2u windowSize = window.getSize();
    //calculate the size of each cell, break window up into grid basically
    const float cellWidth = static_cast<float>(windowSize.x) / width;
    const float cellHeight = static_cast<float>(windowSize.y) / height;

    //loop through cells creating rectangles for each cell
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const int cellNum = y * width + x;
            //create a rectangle for the cell
            addQuad(cells, static_cast<int>(x * cellWidth), static_cast<int>(y * cellHeight),
                    static_cast<int>(cellWidth), static_cast<int>(cellHeight), 255);

            if (maze.cells[cellNum] & WALL_N) {
                //draw north wall
                addQuad(walls, static_cast<int>(x * cellWidth), static_cast<int>(y * cellHeight),
                        static_cast<int>(cellWidth), static_cast<int>(thickness), 0);
            }
            if (maze.cells[cellNum] & WALL_S) {
                //draw south wall
                addQuad(walls, static_cast<int>(x * cellWidth), static_cast<int>((y + 1) * cellHeight - thickness),
                        static_cast<int>(cellWidth), static_cast<int>(thickness), 0);
            }
            if (maze.cells[cellNum] & WALL_E) {
                //draw east wall
                addQuad(walls, static_cast<int>((x + 1) * cellWidth - thickness), static_cast<int>(y * cellHeight),
                        static_cast<int>(thickness), static_cast<int>(cellHeight), 0);
            }
            if (maze.cells[cellNum] & WALL_W) {
                //draw west wall
                addQuad(walls, static_cast<int>(x * cellWidth), static_cast<int>(y * cellHeight),
                        static_cast<int>(thickness), static_cast<int>(cellHeight), 0);
            }
        }
    }
}

void Renderer::addQuad(sf::VertexArray &array, float x, float y, int width, int height, uint8_t color) {
    //create vertices
    sf::Vertex v1, v2, v3, v4;

    v1.position = {x, y};
    v2.position = {x + width, y};
    v3.position = {x, y + height};
    v4.position = {x + width, y + height};

    //create color for the vertices
    v1.color = sf::Color(color, color, color);
    v2.color = sf::Color(color, color, color);
    v3.color = sf::Color(color, color, color);
    v4.color = sf::Color(color, color, color);

    //add the vertices to the array for triangle 1
    array.append(v1);
    array.append(v2);
    array.append(v3);
    //add the vertices to the array for triangle 2
    array.append(v2);
    array.append(v3);
    array.append(v4);


    // const sf::Vertex v1(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)), sf::Color(color, color, color));
    // const sf::Vertex v2(sf::Vector2f(static_cast<float>(x + width), static_cast<float>(y)), sf::Color(color, color, color));
    // const sf::Vertex v3(sf::Vector2f(static_cast<float>(x), static_cast<float>(y + height)), sf::Color(color, color, color));
    // array.append(v1);
    // array.append(v2);
    // array.append(v3);
    //
    // //create triangle 2
    // const sf::Vertex v4(sf::Vector2f(static_cast<float>(x + width), static_cast<float>(y + height)), sf::Color(color, color, color));
    // array.append(v2);
    // array.append(v3);
    // array.append(v4);
}
