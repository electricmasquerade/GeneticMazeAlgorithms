
#include "Renderer.h"
#include <SFML/Graphics.hpp>


// TODO: make this much faster!! vertex array or something, drawing too many rects as of now
void Renderer::renderMaze(const Maze& maze) const {
    //get maze dimensions
    const size_t width = maze.width;
    const size_t height = maze.height;
    if (width <= 0 || height <= 0) {
        return; // Invalid maze dimensions
    }
    //Create a vertex array for white background cells, possibly optional. Otherwise, leave the background clear.
    sf::VertexArray cells(sf::PrimitiveType::Triangles);
    cells.resize(maze.width * maze.height * 6);

    //get the size of the window
    const sf::Vector2u windowSize = window.getSize();
    //calculate the size of each cell, break window up into grid basically
    const float cellWidth = static_cast<float>(windowSize.x) / width;
    const float cellHeight = static_cast<float>(windowSize.y) / height;
    //Use triangles to make rectangles, vertex buffer to render all at once.



    //loop through cells creating rectangles for each cell
    // for (int y = 0; y < height; ++y) {
    //     for (int x = 0; x < width; ++x) {
    //         //get the cell number
    //         const int cellNum = y * width + x;
    //         //create a rectangle for the cell
    //         sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
    //         //set the position of the cell
    //         cell.setPosition({x * cellWidth, y * cellHeight});
    //         //set the color of the cell
    //         cell.setFillColor(sf::Color::White);
    //         window.draw(cell);
    //         //check for walls and draw rectangles for them
    //         if (maze.cells[cellNum] & WALL_N) {
    //             sf::RectangleShape wall(sf::Vector2f(cellWidth, thickness));
    //             wall.setPosition({x * cellWidth, y * cellHeight});
    //             wall.setFillColor(sf::Color::Black);
    //             window.draw(wall);
    //         }
    //         if (maze.cells[cellNum] & WALL_S) {
    //             sf::RectangleShape wall(sf::Vector2f(cellWidth, thickness));
    //             wall.setPosition({x * cellWidth, (y + 1) * cellHeight});
    //             wall.setFillColor(sf::Color::Black);
    //             window.draw(wall);
    //         }
    //         if (maze.cells[cellNum] & WALL_E) {
    //             sf::RectangleShape wall(sf::Vector2f(thickness, cellHeight));
    //             wall.setPosition({(x + 1) * cellWidth, y * cellHeight});
    //             wall.setFillColor(sf::Color::Black);
    //             window.draw(wall);
    //         }
    //         if (maze.cells[cellNum] & WALL_W) {
    //             sf::RectangleShape wall(sf::Vector2f(thickness, cellHeight));
    //             wall.setPosition({x * cellWidth, y * cellHeight});
    //             wall.setFillColor(sf::Color::Black);
    //             window.draw(wall);
    //         }
    //     }
    // }
}

void Renderer::startAnimation(const Maze& maze, const std::vector<Movement>& steps) {
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

void Renderer::updateAnim(const float dt) {
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

void Renderer::buildVertexArrays(const Maze& maze) {//get maze dimensions
    cells.setPrimitiveType(sf::PrimitiveType::Triangles);

    //get maze dimensions
    const size_t width = maze.width;
    const size_t height = maze.height;
    if (width <= 0 || height <= 0) {
        return;
    }
    cells.resize(maze.width * maze.height * 6);
    //init walls
    walls.setPrimitiveType(sf::PrimitiveType::Triangles);
    walls.resize(maze.width * maze.height * 6 * 4); // 4 walls per cell, 6 vertices per wall
}
