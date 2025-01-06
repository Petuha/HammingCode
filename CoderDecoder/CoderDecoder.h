#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "../SignalGenerator/SignalGenerator.h"
#include "../NoiseGenerator/NoiseGenerator.h"
#include "../SignalIdentificator/SignalIdentificator.h"
#include <random>
/*
Класс, который будет моделировать эксперименты
*/
class HammingCodeHandler {
public:
	enum class Plot { plotN = 4, plotM = 2 };
	HammingCodeHandler(std::string bits, int chunksize, bool modified,
		conversionMethod signal_method, double signal_dt, double signal_A,
		double signal_bitDuration, bool signal_polarity,
		double noise_t, double noise_dt, int noise_nu, int noise_dnu,
		noiseForm noise_form, bool noise_polarity, std::vector<double> noise_params,
		int iterations);
	/*
	returns table row elements for current iteration
	if all iterations are done, returns empty vector

	[0]	- номер итерации

	Последовательность
	[1]	- принятая
	[2]	- восстановленная
	[3]	- конечная

	Кол-во бит
	[4]	- искажённые помехой
	[5]	- верно исправленные в закодированной последовательности
	[6]	- верно исправленные в декодированной последовательности

	[7]	- кол-во ошибок в декодированной последовательности
	[8]	- процент ошибок в декодированной последовательности

	Только для модифицированного
	[9]	- кол-во верных срабатываний проверочного бита
	*/
	std::vector<std::string> next();
	std::vector<Dot> plots[(int)Plot::plotN][(int)Plot::plotM]; // all necessary plots
	double trustlevel = -1;
	double min = -1;
	double max = -1;
private:
	void setTrustLevel();
	
	// base parameters

	std::string bits;
	int chunksize;
	bool modified;

	conversionMethod signal_method;
	double signal_dt;
	double signal_A;
	double signal_bitDuration;
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

	int iteration = 0;
	std::vector<std::pair<double, int>> experiments; // errors - randSeed
	std::pair<int, int> greatestCorrectRestored = { -1, -1 };
	std::string coded;
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
	std::uniform_int_distribution<std::mt19937::result_type> rnum;
};