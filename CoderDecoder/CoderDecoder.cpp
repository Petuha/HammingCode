#include "CoderDecoder.h"
/*
bits - кодируема€ последовательность
chunksize - размер блока
modified - модифицированный/немодифицированный код ’эмминга
*/
std::string hammingCode(const std::string& bits, int chunksize, bool modified) {
	return "";
}
/*
bits - декодируема€ последовательность
chunksize - размер блока
modified - модифицированный/немодифицированный код ’эмминга
*/
std::string hammingDecode(const std::string& bits, int chunksize, bool modified) {
	return "";
}

HammingCodeHandler::HammingCodeHandler(const std::string& bits, int chunksize, conversionMethod signal_method, double signal_dt, double signal_A, double signal_bitDuration, bool signal_polarity, double noise_t, double noise_dt, double noise_nu, double noise_dnu, noiseForm noise_form, bool noise_polarity, std::vector<double> params)
{
}
