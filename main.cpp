#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "Generator.h"
#include "Renderer.h"


int main() {
    //sample ImGui code for now
    constexpr unsigned windowWidth = 800;
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowWidth}), "ImGui + SFML = <3");
    window.setFramerateLimit(120);
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    //handle maze generation here
    Generator maze(100, 100);
    Renderer renderer(window, 1.0f);
    renderer.setFramerateLimit(120.0f);

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

                //TODO: fix resizing the maze, looks like crap right now
                renderer.buildVertexArrays(maze.getMaze());
            }
        }
        sf::Time deltaTime = deltaClock.restart();
        ImGui::SFML::Update(window, deltaTime);

        // ImGui window for simulation controls
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::SetNextWindowSize(ImVec2(250, 120));
        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

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

        ImGui::SetNextWindowPos(ImVec2(controls_pos.x, 0));
        ImGui::SetNextWindowBgAlpha(0.5f);
        //ImGui::SetNextWindowSize(ImVec2(200, 100));
        ImGui::Begin("Simulation Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Frame Rate: %.1f FPS", 1.0f / ImGui::GetIO().DeltaTime);
        ImGui::End();


        window.clear(sf::Color::White);
        //Put render code here
        if (animating) {
            renderer.updateAnim(deltaTime.asSeconds());
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