
#include "Renderer.h"

void Renderer::renderMaze(const Maze& maze) {
    int thickness = 2;
    //get maze dimensions
    const int width = maze.width;
    const int height = maze.height;
    //get the size of the window
    const sf::Vector2u windowSize = window.getSize();
    //calculate the size of each cell, break window up into grid basically
    const float cellWidth = static_cast<float>(windowSize.x) / width;
    const float cellHeight = static_cast<float>(windowSize.y) / height;
    //loop through cells creating rectangles for each cell
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            //get the cell number
            const int cellNum = y * width + x;
            //create a rectangle for the cell
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            //set the position of the cell
            cell.setPosition({x * cellWidth, y * cellHeight});
            //set the color of the cell
            cell.setFillColor(sf::Color::White);
            window.draw(cell);
            //check for walls and draw rectangles for them
            if (maze.cells[cellNum] & WALL_N) {
                sf::RectangleShape wall(sf::Vector2f(cellWidth, thickness));
                wall.setPosition({x * cellWidth, y * cellHeight});
                wall.setFillColor(sf::Color::Black);
                window.draw(wall);
            }
            if (maze.cells[cellNum] & WALL_S) {
                sf::RectangleShape wall(sf::Vector2f(cellWidth, thickness));
                wall.setPosition({x * cellWidth, (y + 1) * cellHeight});
                wall.setFillColor(sf::Color::Black);
                window.draw(wall);
            }
            if (maze.cells[cellNum] & WALL_E) {
                sf::RectangleShape wall(sf::Vector2f(thickness, cellHeight));
                wall.setPosition({(x + 1) * cellWidth, y * cellHeight});
                wall.setFillColor(sf::Color::Black);
                window.draw(wall);
            }
            if (maze.cells[cellNum] & WALL_W) {
                sf::RectangleShape wall(sf::Vector2f(thickness, cellHeight));
                wall.setPosition({x * cellWidth, y * cellHeight});
                wall.setFillColor(sf::Color::Black);
                window.draw(wall);
            }
        }
    }
}

void Renderer::startAnimation(const Maze& maze, const std::vector<Movement>& steps) {
    //set the animated maze to the original maze
    animatedMaze.height = maze.height;
    animatedMaze.width = maze.width;
    animatedMaze.cells.resize(animatedMaze.height * animatedMaze.width);
    animatedSteps = steps;
    currentStep = 0;
    accumulator = 0;
}

void Renderer::updateAnim(const float dt) {
    //handle one step of the animation
    accumulator += dt;
    while (accumulator >= dt && currentStep < animatedSteps.size()) {
        //get the current step
        Movement movement = animatedSteps[currentStep];
        //remove the wall in the given direction
        Generator::removeWall(animatedMaze, movement.x, movement.y, movement.direction);
        //increment the step
        currentStep++;
        accumulator -= dt;
    }

}

void Renderer::drawAnim() {
    renderMaze(animatedMaze);
}
