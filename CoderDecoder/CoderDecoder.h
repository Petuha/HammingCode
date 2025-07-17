#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "../SignalGenerator/SignalGenerator.h"
#include "../NoiseGenerator/NoiseGenerator.h"
#include "../SignalIdentificator/SignalIdentificator.h"
#include <random>
#include "IterationResult.h"

/*
Класс, который будет моделировать эксперименты
*/
class HammingCodeHandler {
public:
	HammingCodeHandler() = default;
	HammingCodeHandler(std::string bits, int chunksize, bool modified,
		conversionMethod signal_method, double signal_dt, double signal_A,
		int signal_DotsPerBit, bool signal_polarity,
		double noise_t, double noise_dt, int noise_nu, int noise_dnu,
		noiseForm noise_form, bool noise_polarity, std::vector<double> noise_params,
		int iterations);

	void generate();
	std::vector<Dot> getReveicedOnIteration(int iteration);

	bool modified;
	std::string coded;
	std::vector<IterationResult> iterationResults;
	std::vector<Dot> sent;
	std::vector<Dot> receivedMaxError;
	std::vector<Dot> receivedMinError;
	std::vector<Dot> receivedMedianError;
	std::vector<Dot> receivedMaxCorrectedError;
	double trustlevel = -1;
	double min = -1;
	double max = -1;
private:
	IterationResult getIterationResult(int iteration, std::vector<Dot>& received);
	void setTrustLevel();
	
	// base parameters

	std::string bits;
	int chunksize;

	conversionMethod signal_method;
	double signal_dt;
	double signal_A;
	int signal_DotsPerBit;
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

	std::vector<std::pair<double, int>> experiments; // errors - randSeed
	std::pair<int, int> greatestCorrectRestored = { -1, -1 };
};

/*
Класс, отвечающий за задачи по кодированию и декодированию
*/
class TaskManager {
public:
	TaskManager();
	enum class ModifiedVerdict {
		noError,
		oneError,
		evenError,
		oddError,
		nonModified
	};
	bool hasTasks();
	/*
	Устанавливает значения в вектор строк задания
	возвращает 1, если новое задание утсановлено
	возвращает 0, если новое задание не утсановлено
	*/
	bool newTask();
	bool checkAnswer(const std::string& ans, ModifiedVerdict verdict);
	/*
	Вектор строк задания:
	[0] - последовательность бит
	[1] - тип задачи: кодирование(0), декодирование(1)
	[2] - модифицированный (1) или нет (0) код хэмминга
	*/
	std::vector<std::string> task;
private:
	int task_num = 0;
	std::string ansToTask = "";
	ModifiedVerdict verdictTotask = ModifiedVerdict::nonModified;
	std::mt19937 rng;
};