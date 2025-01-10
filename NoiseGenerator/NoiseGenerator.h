#pragma once
#include "../SignalGenerator/SignalGenerator.h"
#include <vector>


class RandomGenerator {
private:
    std::mt19937 gen;
    std::uniform_int_distribution<double> dist;
public:
    RandomGenerator(unsigned int seed, double minVal, double maxVal)
        : gen(seed), dist(minVal, maxVal) {}

    double generate() {
        return dist(gen);
    }

    void setRange(double minVal, double maxVal) {
        dist = std::uniform_int_distribution<double>(minVal, maxVal);
    }
};

class RandomIntParameter {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<int> dist;

public:
    RandomIntParameter(unsigned int seed, int minVal, int maxVal)
        : gen(seed) {
        dist = std::uniform_real_distribution<int>(minVal, maxVal);
    }

    double get() {
        return dist(gen);
    }
};

enum class noiseForm
{
    RECTANGULAR, // прямоугольник
    SINE,        // синусоида
    PARABOLA     // парабола
};
void generateNoise(int randSeed, std::vector<Dot>& signal, double t, double dt, int nu, int dnu,
	noiseForm form, bool polarity, std::vector<double> params);
