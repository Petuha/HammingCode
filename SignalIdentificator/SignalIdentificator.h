﻿#pragma once
#include "../SignalGenerator/SignalGenerator.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
std::string bitsFromSignal(const std::vector<Dot>& signal,
	conversionMethod method, double dt, double A, double bitDuration, bool polarity);
