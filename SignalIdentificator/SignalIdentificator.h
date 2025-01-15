#pragma once
#include "../SignalGenerator/SignalGenerator.h"
#include <iostream>
#include <cmath>
std::string bitsFromSignal(const std::vector<Dot>& signal,
	conversionMethod method, double dt, double A, int dotsPerBit, bool polarity);
