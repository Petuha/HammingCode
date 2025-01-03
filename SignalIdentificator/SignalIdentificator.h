#pragma once
#include "../SignalGenerator/SignalGenerator.h"
#include <string>
#include <vector>
std::string bitsFromSignal(const std::vector<Dot>& signal,
	conversionMethod method, double dt, double A, double bitDuration, bool polarity);