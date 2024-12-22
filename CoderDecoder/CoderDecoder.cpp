#include "CoderDecoder.h"
/*
bits - последовательность бит
chunksize - размер блока
modified - модифицированный/немодифицированный код ’эмминга
*/
std::string hammingCoder(const std::string& bits, int chunksize, bool modified) {
	auto additionalBits = [&]() -> int {
		int n = 0, x = 1;
		while (x <= chunksize + n) {
			++n;
			x <<= 1;
		}
		return n;
		};
	int codedChunkSize = chunksize + additionalBits() + modified;
	std::string res(bits.size() / chunksize * codedChunkSize, 0);
	auto codeBlock = [&](int i) {
		int cBit = 1;
		int start = i / chunksize * codedChunkSize;
		int x, y;
		for (int ib = 1; ib <= codedChunkSize - modified; ++ib) {
			if (ib == cBit) {
				cBit <<= 1;
				continue;
			}
			res[start + ib - 1 + modified] = bits[i] - '0';
			if (modified) res[start] ^= bits[i] - '0';
			x = ib;
			y = 1;
			while (x) {
				res[start + y - 1 + modified] ^= (bits[i] - '0') * (x & 1);
				x >>= 1;
				y <<= 1;
			}
			++i;
		}
		if (modified) {
			x = 1;
			while (x <= codedChunkSize - modified) {
				res[start] ^= res[x];
				x <<= 1;
			}
		}
		};
	for (int i = chunksize; i <= bits.size(); i += chunksize) {
		codeBlock(i - chunksize);
	}
	for (int i = 0; i < res.size(); ++i) {
		res[i] += '0';
	}
	return res;
}
void hammingErrorRestorer(std::string& bits, int chunksize, bool modified) {
	auto additionalBits = [&]() -> int {
		int n = 0, x = 1;
		while (x <= chunksize + n) {
			++n;
			x <<= 1;
		}
		return n;
		};
	int codedChunkSize = chunksize + additionalBits() + modified;
	auto blockSyndrome = [&](int i) -> int {
		int x, y;
		int syndrome = 0;
		for (int ib = 1; ib <= codedChunkSize - modified; ++ib, ++i) {
			x = ib;
			y = 1;
			while (x) {
				syndrome ^= y * (x & 1 & (bits[i + modified] - '0'));
				x >>= 1;
				y <<= 1;
			}
		}
		return syndrome;
		};
	for (int i = codedChunkSize; i <= bits.size(); i += codedChunkSize) {
		int syndrome = blockSyndrome(i - codedChunkSize);

		// some modified analyze should be here (for correct multi-errors recognition), maybe later

		if (!syndrome) continue;
		int index = i - codedChunkSize + modified + syndrome - 1;
		bits[index] -= '0';
		bits[index] ^= 1;
		bits[index] += '0';
	}
}
std::string hammingErrorCompressor(const std::string& bits, int chunksize, bool modified) {
	auto additionalBits = [&]() -> int {
		int n = 0, x = 1;
		while (x <= chunksize + n) {
			++n;
			x <<= 1;
		}
		return n;
		};
	int codedChunkSize = chunksize + additionalBits() + modified;
	std::string res(bits.size() / codedChunkSize * chunksize, 0);
	auto compressBlock = [&](int i) {
		int cBit = 1;
		int resi = i / codedChunkSize * chunksize;
		i += modified;
		for (int ib = 1; ib <= codedChunkSize - modified; ++ib, ++i) {
			if (ib == cBit) {
				cBit <<= 1;
				continue;
			}
			res[resi] = bits[i];
			++resi;
		}
		};
	for (int i = codedChunkSize; i <= bits.size(); i += codedChunkSize) {
		compressBlock(i - codedChunkSize);
	}
	return res;
}
std::string hammingDecoder(const std::string& bits, int chunksize, bool modified) {
	std::string res = bits;
	hammingErrorRestorer(res, chunksize, modified);
	res = hammingErrorCompressor(res, chunksize, modified);
	return res;
}

HammingCodeHandler::HammingCodeHandler(std::string bits, int chunksize, bool modified,
	conversionMethod signal_method, double signal_dt, double signal_A,
	double signal_bitDuration, bool signal_polarity,
	double noise_t, double noise_dt, int noise_nu, int noise_dnu,
	noiseForm noise_form, bool noise_polarity, std::vector<double> noise_params,
	int iterations) :
	bits(std::move(bits)),
	chunksize(chunksize),
	modified(modified),
	signal_method(signal_method),
	signal_dt(signal_dt),
	signal_A(signal_A),
	signal_bitDuration(signal_bitDuration),
	signal_polarity(signal_polarity),
	noise_t(noise_t),
	noise_dt(noise_dt),
	noise_nu(noise_nu),
	noise_dnu(noise_dnu),
	noise_form(noise_form),
	noise_polarity(noise_polarity),
	noise_params(std::move(noise_params)),
	iterations(iterations)
{

}

std::vector<std::string> HammingCodeHandler::next()
{
	if (iteration == iterations) return {};

	// do experiment

	return std::vector<std::string>();
}
