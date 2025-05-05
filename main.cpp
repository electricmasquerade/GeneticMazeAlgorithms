#include <iostream>
#include <filesystem>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "Generator.h"
#include "Renderer.h"


int main() {
    constexpr unsigned windowWidth = 800;
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowWidth}), "Maze Algorithms");
    window.setFramerateLimit(120);
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    //handle maze generation here

    Generator maze(50, 50);
    Renderer renderer(window, 1.0f);
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
    static bool animating = false;
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

        // ImGui window for simulation controls
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.5f);
        //ImGui::SetNextWindowSize(ImVec2(250, 120));
        ImGui::Begin("Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::InputText("Save As", savePath, IM_ARRAYSIZE(savePath));
        ImGui::SameLine();
        if (ImGui::Button("Save Maze")) {
            if (maze.saveMazeToFile(savePath)) {
                snprintf(message, sizeof(message), "Maze saved to %s", savePath);
            } else {
                snprintf(message, sizeof(message), "Error saving maze to %s", savePath);
            }
        }
        ImGui::InputText("Load From", loadPath, IM_ARRAYSIZE(loadPath));
        ImGui::SameLine();
        if (ImGui::Button("Load Maze")) {
            if (maze.loadMazeFromFile(loadPath)) {
                Maze loadedMaze = maze.getMaze();
                currentMaze = loadedMaze;
                maze.reset();
                maze.setMaze(currentMaze);
                snprintf(message, sizeof(message), "Maze loaded from %s", loadPath);
                renderer.buildVertexArrays(maze.getMaze());
            } else {
                snprintf(message, sizeof(message), "Error loading maze from %s", loadPath);
            }
        }
        const ImVec2 data_pos = ImGui::GetWindowSize();
        ImGui::Text("%s", message);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(data_pos.x, 0));
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        //add button here to generate maze
        ImGui::Checkbox("Visualize Maze Generation", &visualizeGeneration);
        //ImGui::Checkbox("Pause", &animating);
        ImGui::Checkbox("Step Through Animation", &stepThrough);
        if (ImGui::Button("Generate Maze")) {
            maze.generateMaze();
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

        ImGui::SetNextWindowPos(ImVec2(controls_pos.x + data_pos.x, 0));
        ImGui::SetNextWindowBgAlpha(0.5f);
        //ImGui::SetNextWindowSize(ImVec2(200, 100));
        ImGui::Begin("Simulation Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Frame Rate: %.1f FPS", 1.0f / ImGui::GetIO().DeltaTime);
        ImGui::End();


        window.clear(sf::Color::White);
        //Put render code here
        if (animating) {
            renderer.updateGenAnim(deltaTime.asSeconds());
            renderer.drawAnim();
            if (renderer.getAnimationFinished()) {
                animating = false;
            }
        }
        else {
            renderer.renderMaze(maze.getMaze());
        }
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}