#include <iostream>
#include <filesystem>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "Generator.h"
#include "Renderer.h"
#include "SolverAgent.h"


int main() {
    constexpr unsigned windowWidth = 1000;
    constexpr unsigned windowHeight = windowWidth * 0.8;
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Maze Algorithms");
    window.setFramerateLimit(120);
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    //handle maze generation here

    Generator maze(100, 100);
    Renderer renderer(window, 1.0f);
    SolverAgent solver = SolverAgent(maze.getMaze());
    renderer.setFramerateLimit(120.0f);
    Maze currentMaze = maze.getMaze();
    renderer.buildVertexArrays(currentMaze);

    //create maze folders
    std::filesystem::create_directory("mazes");

    //create file path buffers
    static char savePath[128] = "mazes/maze1.mz";
    static char loadPath[128] = "mazes/maze1.mz";
    static char message[128] = "";


    static bool visualizeGeneration = false;
    static bool visualizeSearch = false;
    static bool animating = false;
    static bool searching = false;
    static bool paused = false;
    bool stepOnce = false;
    static bool stepThrough = false;


    while (window.isOpen()) {
        // Process events, including window close
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                // Update the view to the new size of the window
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(visibleArea));
                renderer.buildVertexArrays(maze.getMaze());
            }
        }
        sf::Time deltaTime = deltaClock.restart();
        ImGui::SFML::Update(window, deltaTime);

        //handle maze render stuff, split to side window
        window.clear(sf::Color::White);

        sf::View mazeView = window.getDefaultView();
        mazeView.setViewport(sf::FloatRect{{0.2f, 0.0f}, {0.8f, 1.0f}});
        window.setView(mazeView);

        //Put render code here
        if (animating) {
            renderer.updateGenAnim(deltaTime.asSeconds());
            renderer.drawAnim();
            if (renderer.getAnimationFinished()) {
                animating = false;
            }
        }
        else if (searching) {
            renderer.updateSearchAnim(deltaTime.asSeconds());
            renderer.drawAnim();
            if (renderer.getSearchFinished()) {
                renderer.highlightSolution(maze.getMaze(), solver.getSolution());
                visualizeSearch = false;
            }
        }

        else {
            renderer.renderMaze(maze.getMaze());
        }

        //split controls back to the left of screen
        window.setView(window.getDefaultView());


        // ImGui window for simulation controls
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::SetNextWindowSize({window.getSize().x * 0.2f, windowHeight * 0.2}, ImGuiCond_Always);
        ImGui::Begin("Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::InputText("Save As", savePath, IM_ARRAYSIZE(savePath));
        //ImGui::SameLine();
        if (ImGui::Button("Save Maze")) {
            if (maze.saveMazeToFile(savePath)) {
                snprintf(message, sizeof(message), "Maze saved to %s", savePath);
            } else {
                snprintf(message, sizeof(message), "Error saving maze to %s", savePath);
            }
        }
        ImGui::InputText("Load From", loadPath, IM_ARRAYSIZE(loadPath));
        //ImGui::SameLine();
        if (ImGui::Button("Load Maze")) {
            if (maze.loadMazeFromFile(loadPath)) {
                Maze loadedMaze = maze.getMaze();
                currentMaze = loadedMaze;
                maze.reset();
                maze.setMaze(currentMaze);
                snprintf(message, sizeof(message), "Loaded from %s", loadPath);
                renderer.buildVertexArrays(maze.getMaze());
            } else {
                snprintf(message, sizeof(message), "Error loading maze from %s", loadPath);
            }
        }
        const ImVec2 data_pos = ImGui::GetWindowSize();
        ImGui::Text("%s", message);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0, data_pos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize({window.getSize().x * 0.2f, windowHeight * 0.2}, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        //add button here to generate maze
        ImGui::Checkbox("Visualize Maze Generation", &visualizeGeneration);
        //ImGui::Checkbox("Pause", &animating);
        ImGui::Checkbox("Step Through Animation", &stepThrough);
        if (ImGui::Button("Generate Maze")) {
            maze.generateMaze();
            renderer.setDirty();
            searching = false;
            renderer.buildVertexArrays(maze.getMaze());

            if (visualizeGeneration) {
                renderer.startAnimation(maze.getMaze(), maze.getMovements());
                animating = true;
            }
            else {
                animating = false;
            }
        }

        const ImVec2 controls_pos = ImGui::GetWindowSize();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0, controls_pos.y + data_pos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize({window.getSize().x * 0.2f, windowHeight * 0.2}, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Solver", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Checkbox("Visualize Search", &visualizeSearch);
        ImGui::Text("Solver Agent");
        if (ImGui::Button("Solve Maze")) {
            //solver.setMaze(maze.getMaze());
            solver.reset();
            solver.setStartPosition(0, 0);
            solver.setGoalPosition(maze.getMaze().width - 1, maze.getMaze().height - 1);
            solver.solve();
            renderer.buildVertexArrays(maze.getMaze());
            if (visualizeSearch) {
                renderer.startSearchAnim(maze.getMaze(), solver.getPath());
                searching = true;
            }
            else {
                searching = false;
            }
        }
        if (ImGui::Button("Show Solution")) {
            renderer.highlightSolution(maze.getMaze(), solver.getSolution());
        }
        ImVec2 solverPos = ImGui::GetWindowSize();
        ImGui::End();


        ImGui::SetNextWindowPos(ImVec2(0, controls_pos.y + data_pos.y + solverPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize({window.getSize().x * 0.2f, windowHeight * 0.1}, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.5f);
        //ImGui::SetNextWindowSize(ImVec2(200, 100));
        ImGui::Begin("Simulation Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Frame Rate: %.1f FPS", 1.0f / ImGui::GetIO().DeltaTime);
        ImGui::End();



        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}