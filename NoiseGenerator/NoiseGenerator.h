#pragma once
#include "../SignalGenerator/SignalGenerator.h"
#include <vector>
enum class noiseForm {
	a, // Прямоугольная
	asinbx, // Синусоидальная
	axx // Квадратичная
};
std::vector<Dot> generateNoise(int randSeed, const std::vector<Dot>& signal, double t, double dt, double nu, double dnu,
	noiseForm form, bool polarity, std::vector<double> params);