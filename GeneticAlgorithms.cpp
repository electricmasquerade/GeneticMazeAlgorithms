
#include "GeneticAlgorithms.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <cassert>

GeneticAlgorithms::GeneticAlgorithms(const size_t populationSize, const size_t generationCount, const float crossoverRate, const float mutationRate):
    populationSize(populationSize),
    generationCount(generationCount),
    crossoverRate(crossoverRate),
    mutationRate(mutationRate),
    rng(std::random_device{}()) {
    initPopulation(populationSize);
}


void GeneticAlgorithms::loadMazes(const std::string& folderPath) {
    mazes.clear();
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (!entry.is_regular_file()) continue;
        std::ifstream file(entry.path());
        if (!file) {
            std::cerr << "Failed to open maze file: " << entry.path() << std::endl;
            continue;
        }
        Maze maze;
        size_t width = 0;
        size_t height = 0;
        file.read(reinterpret_cast<char*>(&width), sizeof(width));
        file.read(reinterpret_cast<char*>(&height), sizeof(height));
        maze.cells.resize(width * height);
        maze.width = width;
        maze.height = height;
        file.read(reinterpret_cast<char*>(maze.cells.data()), maze.cells.size() * sizeof(uint8_t));
        //add maze data to cells
        mazes.push_back(std::move(maze));
    }
    std::cout << "Loaded " << mazes.size() << " mazes from " << folderPath << std::endl;
}


float GeneticAlgorithms::evaluate(const Maze &maze, const Chromosome &chromosome) const {
    // evaluate the chromosome's performance on the maze
    // this should return a score based on the maze and the chromosome's genes
    int currentCell = 0; //start at the top left
    const int goalCell = (maze.width - 1) + (maze.height - 1) * maze.width; //goal is bottom right
    bool reachedGoal = false;
    int steps = 0;
    static constexpr int wallMasks[4] = {
        WALL_N,
        WALL_E,
        WALL_S,
        WALL_W
    };

    while (steps < MAX_STEPS_PER_MAZE && !reachedGoal) {
        //calculate heuristic
        const int heuristic = calculateHeuristic(maze, currentCell, goalCell);

        //read wall flags and feed into scoring array

        std::array<float, numInputs> features{};
        //error is here
        const auto walls = maze.cells[currentCell];
        // why though

        for (int direction = 0; direction < 4; ++direction) {
            if (walls & wallMasks[direction]) {
                features[direction] = 1.0f; //wall present
            }
            else {
                features[direction] = 0.0f; //no wall
            }
        }

        features[4] = static_cast<float>(heuristic) / (maze.width + maze.height - 2); //normalize heuristic
        features[5] = 1.0f; //bias term, leave at 1 to keep bias the same

        // create output array to hold the scores for each direction
        std::array<float, numOutputs> outputs{};
        // calculate the scores for each direction
        for (int i = 0; i < numOutputs; ++i) {
            outputs[i] = 0.0f;
            for (int j = 0; j < numInputs; ++j) {
                outputs[i] += chromosome.genes[i * numInputs + j] * features[j];
            }
        }
        // find the direction with the highest score
        int best = 0;
        float bestScore = outputs[0];
        for (int i = 1; i < numOutputs; ++i) {
            if (outputs[i] > bestScore || (outputs[i] == bestScore && std::rand() % 2)) {
                bestScore = outputs[i];
                best = i;
            }
        }

        //move to new cell based on direction unless blocked (waste step)
        //check for neighbor out of bounds
        const int neighborX = currentCell % maze.width + dx[best];
        const int neighborY = currentCell / maze.width + dy[best];
        if (neighborX < 0 || neighborX >= maze.width || neighborY < 0 || neighborY >= maze.height) {
            //out of bounds, waste step
            steps++;
            continue;
        }
        //if direction is blocked, waste step
        if (maze.cells[currentCell] & wallMasks[best]) {
            //blocked, waste step
            steps++;
            continue;
        }
        currentCell = neighborY * maze.width + neighborX; //move to new cell
        if (currentCell == goalCell) {
            reachedGoal = true; //reached the goal
            //std::cout << "One reached the goal" << std::endl;
        }
        steps++;
    }
    //calculate fitness score
    float bonus = maze.width + maze.height;
    float fitness = bonus * reachedGoal + (MAX_STEPS_PER_MAZE - steps) * 0.1f -
                   DISTANCE_BONUS * calculateHeuristic(maze, currentCell, goalCell);
    // float fitness = 1.0f / (1.0f + steps + DISTANCE_BONUS * calculateHeuristic(maze, currentCell, goalCell));

    //const float fitness = (reachedGoal ? GOAL_BONUS : 0.0f) - float(steps) - DISTANCE_BONUS * calculateHeuristic(
                              //maze, currentCell, goalCell);
    // float fitness = reachedGoal
    // ? (GOAL_BONUS + (MAX_STEPS_PER_MAZE - steps))
    // : (MAX_STEPS_PER_MAZE - steps) * 0.5f - DISTANCE_BONUS * calculateHeuristic(maze, currentCell, goalCell);
    return fitness;
}

void GeneticAlgorithms::initPopulation(const size_t populationSize) {
    population.clear();
    this->populationSize = populationSize;
    population.resize(populationSize);
    for (auto &chromosome : population) {
        chromosome.genes.resize(numGenes); // Assuming 20 genes for now
        std::uniform_real_distribution distribution(-1.0f, 1.0f); // Random values between -1 and 1
        for (auto &gene : chromosome.genes) {
            gene = distribution(rng); // Randomly initialize genes
        }
        chromosome.fitness = 0.0f; // Initialize fitness to 0
    }
    std::cout << population.size() << " chromosomes generated" << std::endl;

}

void GeneticAlgorithms::evaluateChromosomes() {
    //reset fitnesses to zero
    for (auto &chromosome : population) {
        chromosome.fitness = 0.0f;
        //test on each maze
        for (const auto &maze : mazes) {
            //evaluate the fitness of the chromosome on the maze
            const float fitness = evaluate(maze, chromosome);
            //add the fitness to the chromosome's fitness
            chromosome.fitness += fitness;
        }
        //normalize the fitness by the number of mazes
        chromosome.fitness /= static_cast<float>(mazes.size());
    }
}

Chromosome GeneticAlgorithms::selectParent() {
    constexpr int tournamentSize = 4; //size of tournament
    //pick a parent with highest fitness from a set of randomly selected chromosomes
    std::uniform_real_distribution<float> distribution(0.0f, population.size() - 1);
    Chromosome bestParent = population[static_cast<int>(distribution(rng))];
    for (int i = 1; i<tournamentSize; ++i) {
        auto challenger = population[static_cast<int>(distribution(rng))];
        if (challenger.fitness > bestParent.fitness) {
            bestParent = challenger;
        }
    }

    // const int numParents = populationSize/10; //select 10% of the population
    // std::vector<Chromosome> selectedParents;
    // selectedParents.reserve(numParents);
    // std::uniform_int_distribution<int> distribution(0, population.size() - 1);
    // for (int i = 0; i < numParents; ++i) {
    //     int index = distribution(rng);
    //     selectedParents.push_back(population[index]);
    // }
    // //select parent with highest fitness
    // Chromosome bestParent = selectedParents[0];
    // for (int i = 1; i < selectedParents.size(); ++i) {
    //     if (selectedParents[i].fitness > bestParent.fitness) {
    //         bestParent = selectedParents[i];
    //     }
    // }

    return bestParent;
}

Chromosome GeneticAlgorithms::crossover(const Chromosome& parent1, const Chromosome& parent2) {
    //crossover between two parents to create a child based on crossover rate.
    //flip a coin for each gene to decide who to take from
    Chromosome child;
    child.genes.resize(numGenes);
    child.fitness = 0.0f;
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (int i = 0; i < numGenes; ++i) {
        float coinflip = distribution(rng);
        if (coinflip < 0.5f) {
            child.genes[i] = parent1.genes[i];
        }
        else {
            child.genes[i] = parent2.genes[i];
        }
    }
    return child;
}

void GeneticAlgorithms::mutate(Chromosome &chromosome) {
    std::normal_distribution<float> distribution(0.0f, 0.1f); //random noise for mutation
    for (auto &gene : chromosome.genes) {
        float coinflip = static_cast<float>(std::rand()) / RAND_MAX;
        if (coinflip < mutationRate) {
            gene += distribution(rng); //add random noise to gene
            //clamp the gene to -1.0f to 1.0f
            if (gene < -1.0f) {
                gene = -1.0f;
            }
            else if (gene > 1.0f) {
                gene = 1.0f;
            }
        }
    }
}

void GeneticAlgorithms::selectBestChromosome() {
    //select the best chromosome from the population, highest fitness
    bestChromosome = population[0];
    for (const auto &chromosome : population) {
        if (chromosome.fitness > bestChromosome.fitness) {
            bestChromosome = chromosome;
        }
    }

}

void GeneticAlgorithms::train() {
    initPopulation(populationSize);
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (size_t generation = 0; generation < generationCount; ++generation) {
        std::cout << "Generation " << generation << std::endl;
        //error is in these two functions
        evaluateChromosomes();
        selectBestChromosome();
        std::cout << "Best Fitness: " << bestChromosome.fitness << std::endl;
        //print best weights
        std::cout << "Best Chromosome: " << std::endl;
        for (const auto &gene : bestChromosome.genes) {
            std::cout << gene << " ";
        }
        std::cout << std::endl;
        //calc avg fitness and print, to check for improvement
        float avgFitness = 0.0f;
        for (const auto &chromosome : population) {
            avgFitness += chromosome.fitness;
        }
        avgFitness /= static_cast<float>(population.size());
        std::cout << "Average Fitness: " << avgFitness << std::endl;

        std::vector<Chromosome> newPopulation;
        newPopulation.reserve(populationSize);
        //push the best chromosome to the new population
        newPopulation.push_back(bestChromosome);
        for (size_t i = 0; i < populationSize - 1; ++i) { //-1 because of best chromosome added
            Chromosome parent1 = selectParent();
            Chromosome parent2 = selectParent();
            //crossover only if random number is less than crossover rate
            float chance = distribution(rng);
            Chromosome child = (chance < crossoverRate) ? crossover(parent1, parent2) : parent1;
            mutate(child);
            newPopulation.push_back(child);
        }
        population.swap(newPopulation);
    }
    std::cout << "Training finished" << std::endl;
}

void GeneticAlgorithms::saveBestChromosome(const std::string &fileName) const {
    //save best chromosome genes to binary file
    std::ofstream file{fileName, std::ios::binary};
    if (!file) {
        std::cerr << "Error opening file for writing: " << fileName << std::endl;
        return;
    }
    //write the best chromosome genes to the file in binary
    for (const auto &gene : bestChromosome.genes) {
        file.write(reinterpret_cast<const char*>(&gene), sizeof(gene));
    }
}

int GeneticAlgorithms::calculateHeuristic(const Maze& maze, const int currentCell, const int targetCell) {
    //return heuristic; which is manhattan distance
    const int currentX = currentCell % maze.width;
    const int currentY = currentCell / maze.width;
    const int targetX = targetCell % maze.width;
    const int targetY = targetCell / maze.width;

    return std::abs(currentX - targetX) + std::abs(currentY - targetY);
}

Chromosome GeneticAlgorithms::getBestChromosome() const {
    return bestChromosome;
}


