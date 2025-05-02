# Maze Solver with Genetic Algorithm

## Project Overview
This project implements a two-part system in C++ to generate and solve mazes using evolutionary computation. First, a depth-first search (DFS) algorithm carves out a random perfect maze. Then, a Genetic Algorithm (GA) evolves a population of agent controllers—encoded as sequences of moves—to navigate from the maze entrance to the exit.

Key goals:
- Reinforce procedural generation techniques in C++
- Explore genome encoding, fitness design, and GA operators from scratch
- Practice real-time visualization and parameter tuning with SFML & ImGui

## Features
- **Depth-First Maze Generator**: Perfect maze creation via recursive backtracker
- **Genome-Based Agents**: Fixed-length move-sequence for each individual
- **Fitness Evaluation**: Distance-based scoring with bonus for reaching the goal
- **GA Components**: Tournament selection, single-point crossover, mutation, and elitism
- **Real-Time Controls**: Adjust population size, mutation rate, genome length, and more via ImGui
- **Visualization**: Render maze structure and overlay best agent path in SFML window

## Project Structure
```
src/
  ├── main.cpp           # Entry point, SFML/ImGui setup & loop
  ├── MazeGenerator.h    # Depth-first carving implementation
  ├── MazeGenerator.cpp
  ├── Population.h       # GA population and evolution logic
  ├── Population.cpp
  ├── Agent.h            # Genome & simulation of agent movement
  ├── Agent.cpp
  ├── Renderer.h         # SFML drawing helpers
  └── Renderer.cpp

include/               # Public headers (if splitting)

assets/                # Fonts, icons (for ImGui)

README.md              # This document
Makefile/CMakeLists.txt
```

## Maze Generation
1. Initialize a grid of cells, all walls intact
2. Perform recursive backtracking from the entry cell
3. Knock down walls between current cell and a random unvisited neighbor
4. Continue until all cells are visited

## Genetic Algorithm Solver
- **Genome**: `std::vector<Move>` of fixed length (e.g. 2× maze cell count)
- **Selection**: Tournament of _k_ individuals
- **Crossover**: Single-point swapping of move subsequences
- **Mutation**: Randomly change moves at a given mutation rate
- **Elitism**: Preserve top-performing genomes each generation
- **Termination**: Max generations or solution reached

## Visualization & Controls
- **SFML**: Draws maze walls and highlights agent path
- **ImGui Panel**:
    - Generate New Maze
    - Initialize/Reset GA
    - Run Single Generation / Run to Completion
    - Sliders: Population Size, Genome Length, Mutation Rate, Max Generations
    - Stats: Current Generation, Best Fitness, Steps Taken

## Dependencies
- C++20 compiler
- [SFML 3.0](https://www.sfml-dev.org/)
- [ImGui-SFML](https://github.com/eliasdaler/imgui-sfml)
- CMake (or Make) for build automation

## Build & Run
1. Clone repository
2. Create build directory: `mkdir build && cd build`
3. Configure and build:
   ```bash
   cmake ..
   make
   ```
4. Launch executable: `./MazeGA`

## Future Extensions
- Variable-length genomes or evolving sequence length
- Hybrid GA + local search (e.g. 2-opt refinement)
- Parallel fitness evaluation via `std::async` or OpenMP
- Alternative maze creation (e.g. Prim’s algorithm) and fitness landscapes

---
Start generating mazes and evolving solvers to see emergent pathfinding behaviors in action!

