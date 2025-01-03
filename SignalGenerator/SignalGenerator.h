#pragma once
#include <vector>
#include <string>
struct Dot {
	double x;
	double y;
};
enum class conversionMethod {
	NRZ, // Non-Return-to-Zero
	MANCH, // Манчестерский
	RZ, // Return-to-Zero
	AMI // Alternate Mark Inversion
};
std::vector<Dot> generateSignalFromBits(const std::string& bits,
	conversionMethod method, double dt, double A, double bitDuration, bool polarity);