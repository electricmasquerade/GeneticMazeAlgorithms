
#include "GeneticAlgorithms.h"
#include <fstream>
#include <algorithm>
#include <iostream>

GeneticAlgorithms::GeneticAlgorithms(size_t populationSize, size_t generationCount, float crossoverRate, float mutationRate):
    populationSize(populationSize),
    generationCount(generationCount),
    crossoverRate(crossoverRate),
    mutationRate(mutationRate),
    rng(std::random_device{}()) {
    initPopulation(populationSize);
}

void GeneticAlgorithms::loadMazes(std::ifstream &mazesFolder) {
    //load mazes from the folder
    std::string fileName;
    while (std::getline(mazesFolder, fileName)) {
        Maze maze;
        std::ifstream mazeFile(fileName, std::ios::binary);
        if (!mazeFile) {
            std::cerr << "Error opening maze file: " << fileName << std::endl;
            continue;
        }
        size_t width = 0;
        size_t height = 0;
        mazeFile.read(reinterpret_cast<char*>(&width), sizeof(width));
        mazeFile.read(reinterpret_cast<char*>(&height), sizeof(height));
        maze.cells.resize(width * height);
        maze.width = width;
        maze.height = height;
        mazeFile.read(reinterpret_cast<char*>(maze.cells.data()), maze.cells.size() * sizeof(uint8_t));
        mazes.push_back(maze);
    }
    mazesFolder.close();
}


float GeneticAlgorithms::evaluate(const Maze &maze, const Chromosome &chromosome) const {
    // evaluate the chromosome's performance on the maze
    //this should return a score based on the maze and the chromosome's genes

    int currentCell = 0; //start at the top left
    const int goalCell = (maze.width - 1) + (maze.height - 1) * maze.width; //goal is bottom right
    bool reachedGoal = false;
    int steps = 0;

    while (steps < MAX_STEPS_PER_MAZE && !reachedGoal) {
        //calculate heuristic
        int heuristic = calculateHeuristic(maze, currentCell, goalCell);
        //read wall flags and feed into scoring array
        std::array<float, numInputs> features{};
        auto walls = maze.cells[currentCell];
        for (int direction = 0; direction < 4; ++direction) {
            features[direction] = (walls & (1 << direction)) ? 1.0f : 0.0f;
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
        if (maze.cells[currentCell] & 1u << best) {
            //blocked, waste step
            steps++;
            continue;
        }
        currentCell = neighborY * maze.width + neighborX; //move to new cell
        if (currentCell == goalCell) {
            reachedGoal = true; //reached the goal
        }
        steps++;
    }
    //calculate fitness score
    const float fitness = (reachedGoal ? GOAL_BONUS : 0.0f) - float(steps);
    return fitness;
}

void GeneticAlgorithms::initPopulation(size_t populationSize) {
    population.resize(populationSize);
    for (auto &chromosome : population) {
        chromosome.genes.resize(numGenes); // Assuming 20 genes for now
        std::uniform_real_distribution distribution(-1.0f, 1.0f); // Random values between -1 and 1
        for (auto &gene : chromosome.genes) {
            gene = distribution(rng); // Randomly initialize genes
        }
        chromosome.fitness = 0.0f; // Initialize fitness to 0
    }

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
    //pick a parent with highest fitness from a set of randomly selected chromosomes
    int numParents = 3;
    std::vector<Chromosome> selectedParents;
    selectedParents.reserve(numParents);
    std::uniform_int_distribution<int> distribution(0, population.size() - 1);
    for (int i = 0; i < numParents; ++i) {
        int index = distribution(rng);
        selectedParents.push_back(population[index]);
    }
    //select parent with highest fitness
    Chromosome bestParent = selectedParents[0];
    for (int i = 1; i < selectedParents.size(); ++i) {
        if (selectedParents[i].fitness > bestParent.fitness) {
            bestParent = selectedParents[i];
        }
    }

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
    initPopulation(numGenes);
    for (size_t generation = 0; generation < generationCount; ++generation) {
        evaluateChromosomes();
        selectBestChromosome();
        std::vector<Chromosome> newPopulation;
        newPopulation.reserve(populationSize);
        for (size_t i = 0; i < populationSize; ++i) {
            Chromosome parent1 = selectParent();
            Chromosome parent2 = selectParent();
            Chromosome child = crossover(parent1, parent2);
            mutate(child);
            newPopulation.push_back(child);
        }
        population.swap(newPopulation);
    }
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


