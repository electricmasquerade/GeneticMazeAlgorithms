# Maze Generator & Genetic Solver

A C++17 sandbox that can **generate**, **render**, and **solve** perfect mazes in real time. The goal was to try and 
build a genetic algorithm that could train and solve mazes, but that bit is not quite working right now. It doesn't 
train super well and barely solves any mazes. Not sure if I'll be able to fix it since I don't quite know enough 
to fine-tune it. Not a total loss though, since the maze generation and rendering parts are working great. A* algo solves 
mazes perfectly too, so it's a big improvement over my previous attempt in Python. Way way faster too. Also first time 
building a GUI this in depth, which I look forward to using again. 


It bundles three core modules:

* **Generator** – builds perfect mazes (depth‑first search).
* **Renderer** – draws them with SFML + ImGui, so you can watch each step at up to 240 FPS. Probably more but that's what 
I capped it at. 
* **Solver** – run either A\* (deterministic) or a tiny Genetic‑Algorithm agent that learns a move policy.


## Using the App

| Panel        | Purpose                                            |
| ------------ | -------------------------------------------------- |
| **Settings** | Pick maze size, generate, toggle animations        |
| **Solver**   | run A\*, or let the GA try      |
| **GA**       | Train on a maze set, save/load the best chromosome |

---
All code happens on CPU, so having a good GPU won't make anything faster sadly. That's a problem for later.


## File Layout

```
mazes/            # saved mazes (.mz)
train_mazes/      # GA training set
test_mazes/       # GA evaluation set
best_chromosome.bin
```

`.mz` is a tiny binary: width, height, then one byte per cell (walls = N1|S2|E4|W8). My first time using bitmasks for 
stuff, but it was surprisingly fairly easy and works insanely fast. 

## Roadmap
- [ ] Fix the GA solver (maybe)
- [ ] Optimize - parallelize batch generating and GA training (unlikely to happen)



