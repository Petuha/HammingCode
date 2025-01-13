#include "NoiseGenerator.h"

double getImpulseValue(
	noiseForm form,
	double localX,         // локальная координата внутри импульса (0..impulseLength)
	double impulseLength,  // общая длина импульса
	double aVal,           // параметр a (случайно выбранный из [a_mean - a_delta, a_mean + a_delta])
	double bVal            // параметр b (если не нужен, можно 0)
)
{
	switch (form)
	{
	case noiseForm::RECTANGULAR:
		// Прямоугольник: на всем участке [0..impulseLength] значение = aVal
		return aVal;

	case noiseForm::SINE:
	{
		// Синусоида: a * sin(b*x)
		// Здесь localX - наш "x", bVal - наш "b", aVal - коэффициент
		// Можно дополнительно масштабировать аргумент sin, если нужно, 
		// но условие указывает прямо a·sin(b·x).
		double value = aVal * std::sin(bVal * localX);
		return value;
	}

	case noiseForm::PARABOLA:
	{
		// Парабола: a * ( (x - t/2)^2 - (t^2)/4 )
		// где t = impulseLength.
		// Такая функция обнуляется в x=0 и x=t, давая "горб" или "впадину".
		double shift = impulseLength / 2.0; // t/2
		double value = aVal * ((localX - shift) * (localX - shift) - (impulseLength * impulseLength) / 4.0);
		return value;
	}
	}
	return 0.0;
}

void generateNoise(int randSeed, std::vector<Dot>& signal, double t, double dt, int nu, int dnu,
	noiseForm form, bool polarity, std::vector<double> params)
{
	int signalSize = static_cast<int>(signal.size());
	if (signalSize <= 1) return;

	double aMean = params[0];
	double aDelta = params[1];
	double bMean = params.size() == 4 ? params[2] : 0.0;
	double bDelta = params.size() == 4 ? params[3] : 0.0;

	double aMin, aMax;
	if (!polarity) {
		if (aMean > 0) {
			aMin = std::max(aMean - aDelta, 0.0);
			aMax = aMean + aDelta;
		}
		else {
			aMin = aMean - aDelta;
			aMax = std::min(aMean + aDelta, 0.0);
		}
	}
	else {
		aMin = aMean - aDelta;
		aMax = aMean + aDelta;
	}

	std::mt19937 rng(randSeed);
	std::uniform_real_distribution<double> tRand(std::max(0.0, t - dt), t + dt);
	std::uniform_real_distribution<double> timeInSecRand(0.0, 1.0);
	std::uniform_int_distribution nuRand(std::max(0, nu - dnu), nu + dnu);
	std::uniform_real_distribution<double> aRand(aMin, aMax);
	std::uniform_int_distribution signRand(polarity ? 0 : 1, 1);
	std::uniform_real_distribution<double> bRand(bMean - bDelta, bMean + bDelta);

	int sign;
	double aVal, bVal;

	int numImpulses;
	double impulseLength, localX, startTime, endTime;


	double deltaT = signal[1].x - signal[0].x;
	int endSecond = static_cast<int>(std::floor(signal.back().x));

	for (int sec = 0; sec <= endSecond; ++sec) {
		numImpulses = nuRand(rng);
		for (int i = 0; i < numImpulses; ++i) {
			startTime = sec + timeInSecRand(rng);
			impulseLength = tRand(rng);
			endTime = startTime + impulseLength;
			sign = signRand(rng) == 1 ? 1 : -1;
			aVal = aRand(rng);
			bVal = bRand(rng);
			for (int j = static_cast<int>(std::ceil(startTime / deltaT));
				j < signalSize && signal[j].x < endTime; ++j) {
				localX = signal[i].x - startTime;
				signal[j].y += sign * getImpulseValue(form, localX, impulseLength, aVal, bVal);
			}
		}
	}
}
