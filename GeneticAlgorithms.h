
#ifndef GENETICALGORITHMS_H
#define GENETICALGORITHMS_H
#include <random>
#include <vector>

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
    GeneticAlgorithms(int populationSize, int generationCount, int mutationRate);
    ~GeneticAlgorithms() = default;

    void evaluateChromosomes();
    void selectParent();
    void crossover();
    void mutate();
    void selectBestChromosome();
    void train();
    void saveBestChromosome(const std::string& fileName) const;

    [[nodiscard]] Chromosome getBestChromosome() const;


private:
    int populationSize;
    int generationCount;
    int mutationRate;
    std::vector<Chromosome> population;
    Chromosome bestChromosome;



    std::mt19937 rng;




};



#endif //GENETICALGORITHMS_H
