
#ifndef GENETICALGORITHMS_H
#define GENETICALGORITHMS_H
#include <random>
#include <vector>
#include <__filesystem/directory_iterator.h>

#include "Generator.h"

/*
 * this class handles genetic algos and training the agent to solve mazes with policy
 * vectors (not a series of movements, should be a generic solver)
 * for now, policy is 4 sensors for input and the manhattan distance to goal
 * outputs a score for each direction
 */

struct Chromosome {
    //store dna for evolution
    std::vector<float> genes; //linear weights to apply to choices, should be about 20 (5 input * 4 outputs)
    float fitness{0}; //fitness score for the chromosome

};

class GeneticAlgorithms {
public:
    GeneticAlgorithms(size_t populationSize, size_t generationCount, float crossoverRate, float mutationRate);
    ~GeneticAlgorithms() = default;

    void loadMazes(const std::string& folderPath);

    [[nodiscard]] float evaluate(const Maze& maze, const Chromosome& chromosome) const;
    void initPopulation(size_t populationSize);
    void evaluateChromosomes();
    Chromosome selectParent();
    Chromosome crossover(const Chromosome& parent1, const Chromosome& parent2);
    void mutate(Chromosome& chromosome);
    void selectBestChromosome();
    void train();
    void saveBestChromosome(const std::string& fileName) const;

    static int calculateHeuristic(const Maze& maze, int currentCell, int targetCell);

    [[nodiscard]] Chromosome getBestChromosome() const;
    void setGenerationCount(size_t generationCount){this->generationCount = generationCount;}
    [[nodiscard]] size_t getGenerationCount() const {return generationCount;}
    [[nodiscard]] size_t getPopulationSize() const{return populationSize;}
    void setPopulationSize(size_t populationSize) {this->populationSize = populationSize;}
    [[nodiscard]] const std::vector<Chromosome>& getPopulation() const{return population;}
    [[nodiscard]] const std::vector<Maze>& getMazes() const {return mazes;}



private:
    size_t populationSize;
    size_t generationCount;
    float crossoverRate{0.5}; //probability of crossover
    float mutationRate;
    std::vector<Chromosome> population;
    Chromosome bestChromosome;

    std::vector<Maze> mazes;

    static constexpr size_t MAX_GENERATIONS = 1000;
    static constexpr size_t MAX_POPULATION = 100;
    static constexpr size_t MAX_STEPS_PER_MAZE = 1000;
    static constexpr int GOAL_BONUS = 1000; //bonus for reaching the goal
    static constexpr float DISTANCE_BONUS = 0.5; //bonus for distance to goal, smaller is better
    static constexpr size_t numInputs = 6; //number of inputs, 4 walls plus heuristic + bias term
    static constexpr size_t numOutputs = 4; //num of outputs, which is just the 4 directions
    static constexpr size_t numGenes = numInputs * numOutputs; //number of genes in the chromosome


    std::mt19937 rng;

    // direction arrays
    //   0 = Up    (north)
    //   1 = Right (east)
    //   2 = Down  (south)
    //   3 = Left  (west)
    static constexpr int dx[4] = {  0, +1,  0, -1 }; //index these to move left, right, etc.
    static constexpr int dy[4] = { -1,  0, +1,  0 };

    //add mask for walls here.
    static constexpr uint8_t WALL_N = 1 << 0;
    static constexpr uint8_t WALL_S = 1 << 1;
    static constexpr uint8_t WALL_E = 1 << 2;
    static constexpr uint8_t WALL_W = 1 << 3;

};



#endif //GENETICALGORITHMS_H
