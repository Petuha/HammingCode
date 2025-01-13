#pragma once
#include "../SignalGenerator/SignalGenerator.h"
#include <vector>

enum class noiseForm
{
    RECTANGULAR, // прямоугольник
    SINE,        // синусоида
    PARABOLA     // парабола
};
void generateNoise(int randSeed, std::vector<Dot>& signal, double t, double dt, int nu, int dnu,
	noiseForm form, bool polarity, std::vector<double> params);
