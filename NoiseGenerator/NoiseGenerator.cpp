#include "NoiseGenerator.h"
/*
signal - входной сигнал
t - среднее значение длительности
dt - отклонение от среднего значения длительности
nu - среднее значение частоты повторения (количество точек, в которых начинается помеха, за единицу времени)
dnu - отклонение от среднего значения частоты
form - форма импульса
polarity - полярность (0 - однополярная, 1 - биполярная)
params - дополнительные параметры для формы импульса
*/
std::vector<Dot> generateNoise(int randSeed, const std::vector<Dot>& signal, double t, double dt, int nu, int dnu,
    noiseForm form, bool polarity, std::vector<double> params)
{
    return std::vector<Dot>();
}
