#include "CoderDecoder.h"

// !!! must be done
double getTrustLevel(int iterations) {

	return 0;
}

int getcodedChunkSize(int chunksize, bool modified) {
	auto additionalBits = [&]() -> int {
		int n = 0, x = 1;
		while (x <= chunksize + n) {
			++n;
			x <<= 1;
		}
		return n;
		};
	return chunksize + additionalBits() + modified;
}
/*
bits - последовательность бит
chunksize - размер блока
modified - модифицированный/немодифицированный код Хэмминга
*/
std::string hammingCoder(const std::string& bits, int chunksize, bool modified) {
	int codedChunkSize = getcodedChunkSize(chunksize, modified);
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
void hammingErrorRestorer(std::string& bits, int chunksize, bool modified,
	int* ErrorPtr = 0, std::string* correctStringPtr = 0, TaskManager::ModifiedVerdict* verdict = 0) {
	int codedChunkSize = getcodedChunkSize(chunksize, modified);
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
		if (modified) {
			int syndromeP = 0;
			if (ErrorPtr && correctStringPtr) {
				int errors = 0;
				for (int j = i - codedChunkSize; j < i; ++j) {
					errors += bits[j] != (*correctStringPtr)[j];
					syndromeP ^= bits[j] - '0';
				}
				if (syndromeP == 0 && syndrome == 0) {
					*ErrorPtr += errors == 0;
				}
				else if (syndromeP != 0 && syndrome == 0) {
					*ErrorPtr += errors == 1;
				}
				else if (syndromeP == 0 && syndrome != 0) {
					*ErrorPtr += errors % 2 == 0;
				}
				else {
					*ErrorPtr += errors % 2 != 0;
				}
			}
			else {
				for (int j = i - codedChunkSize; j < i; ++j) {
					syndromeP ^= bits[j] - '0';
				}
			}
			if (verdict) {
				if (syndromeP == 0 && syndrome == 0) {
					*verdict = TaskManager::ModifiedVerdict::noError;
				}
				else if (syndromeP != 0 && syndrome == 0) {
					*verdict = TaskManager::ModifiedVerdict::oneError;
				}
				else if (syndromeP == 0 && syndrome != 0) {
					*verdict = TaskManager::ModifiedVerdict::evenError;
				}
				else {
					*verdict = TaskManager::ModifiedVerdict::oddError;
				}
			}
		}
		if (!syndrome) continue;
		int index = i - codedChunkSize + modified + syndrome - 1;
		bits[index] -= '0';
		bits[index] ^= 1;
		bits[index] += '0';
	}
}
std::string hammingErrorCompressor(const std::string& bits, int chunksize, bool modified) {
	int codedChunkSize = getcodedChunkSize(chunksize, modified);
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
std::string hammingDecoder(const std::string& bits, int chunksize, bool modified,
	TaskManager::ModifiedVerdict* verdict = 0) {
	std::string res = bits;
	hammingErrorRestorer(res, chunksize, modified, 0, 0, verdict);
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
	experiments.reserve(iterations);
	coded = hammingCoder(this->bits, chunksize, modified);
	trustlevel = getTrustLevel(iterations);
}

std::vector<std::string> HammingCodeHandler::next()
{
	if (iteration == iterations) return {};

	// do experiment

	/*
	Данные строки таблицы

	номер итерации

	последовательности:

	принятая
	с восстановленными ошибками
	конечная

	далее информация:

	кол-во искажённых помехой бит
	кол-во верно исправленных бит в закодированной последовательности
	кол-во верно исправленных бит в декодированной последовательности
	кол-во ошибок в декодированной последовательности
	процент ошибок в декодированной последовательности

	Если код модифицированный:

	кол-во верных вердиктов добавочного бита
	*/
	std::vector<std::string> data(8 + modified);

	std::vector<Dot> signal = generateSignalFromBits
	(coded, signal_method, signal_dt, signal_A, signal_bitDuration, signal_polarity);
	signal = generateNoise
	(iteration, signal, noise_t, noise_dt, noise_nu, noise_dnu, noise_form, noise_polarity, noise_params);
	std::string receivedBits = bitsFromSignal
	(signal, signal_method, signal_dt, signal_A, signal_bitDuration, signal_polarity);

	if (receivedBits.size() != coded.size()) return {}; // something really bad, bitsFromSignal problem

	int brokenBits = 0;
	for (int i = 0; i < receivedBits.size(); ++i) {
		brokenBits += coded[i] != receivedBits[i];
	}

	std::string receivedBitsRestored = receivedBits;
	int correctModifiedVerdicts = 0;
	if (modified) {
		hammingErrorRestorer(receivedBitsRestored, chunksize, modified, &correctModifiedVerdicts, &coded);
	}
	else {
		hammingErrorRestorer(receivedBitsRestored, chunksize, modified);
	}

	int correctRestoredBitsCoded = 0;
	for (int i = 0; i < receivedBits.size(); ++i) {
		correctRestoredBitsCoded += receivedBits[i] != receivedBitsRestored[i] && receivedBitsRestored[i] == coded[i];
	}

	std::string decodedBits = hammingErrorCompressor(receivedBitsRestored, chunksize, modified);

	std::string compressedReceivedBits = hammingErrorCompressor(receivedBits, chunksize, modified);
	int correctRestoredBitsDecoded = 0;
	for (int i = 0; i < compressedReceivedBits.size(); ++i) {
		correctRestoredBitsDecoded += decodedBits[i] != compressedReceivedBits[i] && compressedReceivedBits[i] == bits[i];
	}
	greatestCorrectRestored = std::max(greatestCorrectRestored, std::make_pair(correctRestoredBitsDecoded, iteration));
	// free memory
	compressedReceivedBits.clear();
	compressedReceivedBits.shrink_to_fit();

	int errorsDecoded = 0;
	for (int i = 0; i < decodedBits.size(); ++i) {
		errorsDecoded += decodedBits[i] != bits[i];
	}

	double errorPercent = 1.0 * errorsDecoded / decodedBits.size();

	data[0] = std::to_string(iteration); // номер итерации
	data[1] = std::move(receivedBits); // принятая
	data[2] = std::move(receivedBitsRestored); // восстановленная
	data[3] = std::move(decodedBits); // конечная
	data[4] = std::to_string(brokenBits); // искажённые помехой
	data[5] = std::to_string(correctRestoredBitsCoded); // верно исправленные в закодированной последовательности
	data[6] = std::to_string(correctRestoredBitsDecoded); // верно исправленные в декодированной последовательности
	data[7] = std::to_string(errorsDecoded); // ошибки в декодированной последовательности
	data[8] = std::to_string(errorPercent); // процент ошибок в декодированной последовательности
	if (modified) data[9] = std::to_string(correctModifiedVerdicts); // верные срабатывания проверочного бита


	experiments.push_back({ errorPercent, iteration });
	++iteration;

	if (iteration == iterations) {
		// prepare plots
		sort(experiments.begin(), experiments.end());
		auto addPlot = [&](int i, int seed) {
			plots[i][0] = generateSignalFromBits
			(bits, signal_method, signal_dt, signal_A, signal_bitDuration, signal_polarity);
			plots[i][0] = generateNoise
			(seed, plots[i][0], noise_t, noise_dt, noise_nu, noise_dnu, noise_form, noise_polarity, noise_params);

			plots[i][1] = generateSignalFromBits
			(coded, signal_method, signal_dt, signal_A, signal_bitDuration, signal_polarity);
			plots[i][1] = generateNoise
			(seed, plots[i][1], noise_t, noise_dt, noise_nu, noise_dnu, noise_form, noise_polarity, noise_params);
			};
		addPlot(0, experiments.rbegin()->second);
		addPlot(1, experiments.begin()->second);
		addPlot(2, experiments[experiments.size() / 2].second);
		addPlot(3, greatestCorrectRestored.second);
	}

	return data;
}

TaskManager::TaskManager() :
	rng(std::mt19937(time(0))),
	rnum(std::uniform_int_distribution<std::mt19937::result_type>(4, 20)),
	task(std::vector<std::string>(3))
{
	task[1] = task[2] = "0";
}

bool TaskManager::hasTasks()
{
	return task_num < 4;
}

bool TaskManager::newTask()
{
	if (task_num >= 4 || ansToTask != "") return 0;
	auto randBits = [&]() -> std::string {
		int n = rnum(rng);
		std::string ret(n, '0');
		for (int i = 0; i < n; ++i) {
			ret[i] = '0' + rnum(rng) % 2;
		}
		return ret;
		};
	task[0] = randBits();
	task[1][0] = '0' + task_num % 2;
	task[2][0] = '0' + task_num / 2 % 2;
	ModifiedVerdict verdict = ModifiedVerdict::nonModified;
	if (task[2][0] == '1') {
		if (task[1][0] == '0') {
			ansToTask = hammingCoder(task[0], task[0].size(), 1);
		}
		else {
			ansToTask = hammingDecoder(task[0], task[0].size(), 1, &verdict);
		}
	}
	else {
		if (task[1][0] == '0') {
			ansToTask = hammingCoder(task[0], task[0].size(), 0);
		}
		else {
			ansToTask = hammingDecoder(task[0], task[0].size(), 0);
		}
	}
	return 1;
}

bool TaskManager::checkAnswer(const std::string& ans, ModifiedVerdict verdict)
{
	if (ans == ansToTask && verdict == verdictTotask) {
		++task_num;
		ansToTask = "";
		return 1;
	}
	return 0;
}
