#include "NoiseGenerator.h"
/*
signal - входной сигнал
t - среднее значение длительности
dt - отклонение от среднего значения длительности
nu - среднее значение частоты повторения
dnu - отклонение от среднего значения частоты
form - форма импульса
polarity - полярность (0 - однополярная, 1 - биполярная)
params - дополнительные параметры для формы импульса
*/
std::vector<Dot> generateNoise(const std::vector<Dot>& signal, double t, double dt, double nu, double dnu,
    noiseForm form, bool polarity, std::vector<double> params)
{
    return std::vector<Dot>();
}
