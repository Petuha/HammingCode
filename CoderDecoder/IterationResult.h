#pragma once
#include <string>

struct IterationResult
{
public:
	std::string receivedBits;
	std::string restoredBits;
	std::string decodedBits;
	int brokenCount;
	int correctlyRestoredInEncodedCount;
	int correctlyRestoredInDecodedCount;
	int errorsInDecodedCount;
	double errorsInDecodedRatio;
	int correctModifiedVerdictCount;
};