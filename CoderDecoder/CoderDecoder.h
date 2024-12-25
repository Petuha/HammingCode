#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "../SignalGenerator/SignalGenerator.h"
#include "../NoiseGenerator/NoiseGenerator.h"
#include "../SignalIdentificator/SignalIdentificator.h"
/*
Класс, который будет моделировать эксперименты
*/
class HammingCodeHandler {
public:
	enum class Plot { plotN = 4, plotM = 2 };
	HammingCodeHandler(std::string bits, int chunksize, bool modified,
		conversionMethod signal_method, double signal_dt, double signal_A,
		double signal_bitDuration, bool signal_polarity,
		double noise_t, double noise_dt, int noise_nu, int noise_dnu,
		noiseForm noise_form, bool noise_polarity, std::vector<double> noise_params,
		int iterations);
	/*
	returns table row elements for current iteration
	if all iterations are done, returns empty vector
	*/
	std::vector<std::string> next();
	std::vector<Dot> plots[(int)Plot::plotN][(int)Plot::plotM]; // all necessary plots
	double trustlevel;
private:
	// base parameters

	std::string bits;
	int chunksize;
	bool modified;

	conversionMethod signal_method;
	double signal_dt;
	double signal_A;
	double signal_bitDuration;
	bool signal_polarity;

	double noise_t;
	double noise_dt;
	int noise_nu;
	int noise_dnu;
	noiseForm noise_form;
	bool noise_polarity;
	std::vector<double> noise_params;

	int iterations;

	// other parameters

	int iteration = 0;
	std::vector<std::pair<double, int>> experiments; // errors - randSeed
	std::pair<int, int> greatestCorrectRestored = { -1, -1 };
	std::string coded;
};