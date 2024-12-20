#pragma once
#include <string>
#include <vector>
#include "../SignalGenerator/SignalGenerator.h"
#include "../NoiseGenerator/NoiseGenerator.h"
/*
Класс, который будет моделировать эксперименты
*/
class HammingCodeHandler {
private:
public:
	HammingCodeHandler(const std::string& bits, int chunksize,
		conversionMethod signal_method, double signal_dt, double signal_A,
		double signal_bitDuration, bool signal_polarity,
		double noise_t, double noise_dt, double noise_nu, double noise_dnu,
		noiseForm noise_form, bool noise_polarity, std::vector<double> params);

};