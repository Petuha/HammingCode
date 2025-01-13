﻿#include "NoiseGenerator.h"
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

class RandomDouble {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> dist;

public:
    RandomDouble(unsigned int seed, double minVal, double maxVal)
       : gen(seed), dist(minVal, maxVal) {}

    double get() {
        return dist(gen);
    }
};

class RandomInt{
private:
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;

public:
    RandomInt(unsigned int seed, int minVal, int maxVal)
        : gen(seed), dist(minVal, maxVal) {}

    int get() {
        return dist(gen);
    }
};

double getImpulseValue(
    ImpulseForm form,
    double localX,         // локальная координата внутри импульса (0..impulseLength)
    double impulseLength,  // общая длина импульса
    double aVal,           // параметр a (случайно выбранный из [a_mean - a_delta, a_mean + a_delta])
    double bVal            // параметр b (если не нужен, можно 0)
)
{
    switch (form)
    {
    case ImpulseForm::RECTANGULAR:
        // Прямоугольник: на всем участке [0..impulseLength] значение = aVal
        return aVal;

    case ImpulseForm::SINE:
    {
        // Синусоида: a * sin(b*x)
        // Здесь localX - наш "x", bVal - наш "b", aVal - коэффициент
        // Можно дополнительно масштабировать аргумент sin, если нужно, 
        // но условие указывает прямо a·sin(b·x).
        double value = aVal * std::sin(bVal * localX);
        return value;
    }

    case ImpulseForm::PARABOLA:
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
    if (signal.size() <= 1)
    {
        // Если меньше двух точек, нет смысла что-то делать.
        return;
    }


    RandomDouble tRand(seed++, std::max(0.0, t - dt), t + dt);   // Для длительности
    RandomDouble timeInSecRand(seed++, 0.0, 1.0);        // Для начала импульса в секунде
    RandomInt nuRand(seed++, std::max(0, nu - dnu), nu + dnu);  // Для количества импульсов

    int sign = 1;
    // Параметры a
    double aMean = params[0];
    double aDelta = params[1];
    
    // Параметры b (если есть)
    double bMean = params.size() == 4 ? params[2] : 0.0;
    double bDelta = params.size() == 4 ? params[3] : 0.0;
    RandomDouble bRand(0, 0.0, 1.0);  // Временная инициализация
    double bVal = 0.0;
    if (params.size() == 4) {
        bRand = RandomDouble(seed++, bMean - bDelta, bMean + bDelta);
        bVal = bRand.get();
    }
    // Определяем временной диапазон сигнала: от xMin до xMax
    double xMin = signal.front().x;
    double xMax = signal.back().x;

    double deltaT = signal[1].x - signal[0].x;
    
    // Целое кол-во секунд – пробежимся от floor(xMin) до floor(xMax).
    int startSecond = static_cast<int>(std::floor(xMin));
    int endSecond = static_cast<int>(std::floor(xMax));

    // Определяем диапазон для параметра a
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
    
    RandomDouble aRand(seed++, aMin, aMax);
    double aVal = aRand.get();
    
    RandomInt signRand(seed++, polarity ? 0 : 1, 1);
    // Определение знака при polarity == 1 псевдослучайным образом
    sign = signRand.get() ? 1 : -1;
    
    size_t left = 0, right = 0;
    double leftIndex = 0;
    for (int sec = startSecond; sec <= endSecond; ++sec) {
        
        // Двигаем left до тех пор, пока не дойдём до точки,
        // которая впервые >= sec (т.е. входит в данную секунду).
        while (left < signal.size() && signal[left].x < sec) {
            ++left;
        }
        if (left >= signal.size()) {
            break; // выходим, дальше секунд уже нет в сигнале
        }
        // Теперь двигаем right до тех пор, пока не встретим точку >= sec+1
        // (т.е. конец этой секунды)
        right = left; 
        while (right < signal.size() && signal[right].x < sec + 1.0) {
            ++right;
        }

        // Количество импульсов в текущей секунде
        int numImpulses = nuRand.get();

        for (int i = 0; i < numImpulses; ++i) {
            double startTime = sec + timeInSecRand.get();
            double impulseLength = tRand.get();

            if (impulseLength <= 0.0) continue;

            double endTime = startTime + impulseLength;
            
            // Вычисление начального индекса на основе startTime
           
            if (startTime <= xMax) {
                // Вычисление индекса точки, где начинается импульс
                leftIndex = std::ceil((startTime/deltaT);
                if (leftIndex >= signal.size()) {
                    continue;  // Если индекс выходит за границы сигнала, пропускаем импульс
                }
            } else {
                continue;  // startTime вне диапазона сигнала
            }

            // Применение импульса к точкам, начиная с leftIndex, пока x < endTime
            for (size_t i = leftIndex; i < signal.size() && signal[i].x < endTime; ++i) {
                double localX = signal[i].x - startTime;
                // Применяем значение импульса к точке signal[i]
                signal[i].y += sign * getImpulseValue(form, localX, impulseLength, aVal, bVal);
            }
        }
    }
}
