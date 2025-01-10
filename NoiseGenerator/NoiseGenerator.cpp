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

class RandomDouble {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> dist;

public:
    RandomDouble(unsigned int seed, double minVal, double maxVal)
        : gen(seed) {
        dist = std::uniform_real_distribution<double>(minVal, maxVal);
    }

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
        : gen(seed) {
        dist = std::uniform_int_distribution<int>(minVal, maxVal);
    }

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

    // Инициализируем псевдо рандомные числа, используя заданный randSeed
    std::mt19937 gen(randSeed);

    RandomDouble tRand(seed++, std::max(0.0, t - dt), t + dt);   // Для длительности
    RandomDouble timeInSecRand(seed++, 0.0, 1.0);        // Для начала импульса в секунде
    RandomInt nuRand(seed++, std::max(0, nu - dnu), nu + dnu);  // Для количества импульсов

    // Параметры a
    double aMean = params[0];
    double aDelta = params[1];

    // Параметры b (если есть)
    double bMean = params.size() == 4 ? params[2] : 0.0;
    double bDelta = params.size() == 4 ? params[3] : 0.0;
    // Определяем временной диапазон сигнала: от xMin до xMax
    double xMin = signal.front().x;
    double xMax = signal.back().x;
    // Целое кол-во секунд – пробежимся от floor(xMin) до floor(xMax).
    int startSecond = static_cast<int>(std::floor(xMin));
    int endSecond = static_cast<int>(std::floor(xMax));
    
    for (int sec = startSecond; sec <= endSecond; ++sec) {
        // Количество импульсов в текущей секунде
        int numImpulses = static_cast<int>(nuRand.get());

        for (int i = 0; i < numImpulses; ++i) {
            double startTime = sec + timeInSecRand.get();
            double impulseLength = tRand.get();

            if (impulseLength <= 0.0) continue;

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

            // Генератор для a
            RandomDouble aRand(seed++, aMin, aMax);
            double aVal = aRand.get();

            // Генератор для b (если используется)
            double bVal = 0.0;
            if (params.size() == 4) {
                RandomDouble bRand(seed++, bMean - bDelta, bMean + bDelta);
                bVal = bRand.get();
            }

            // Определение знака при polarity == 1 псевдослучайным образом
            double sign = 1.0;
            if (polarity) {
                RandomDouble signRand(seed++, 0.0, 1.0);
                sign = (signRand.get() >= 0.5) ? 1.0 : -1.0;
            }

            double endTime = startTime + impulseLength;

            // Находим диапазон индексов точек, попадающих в [startTime, endTime)
            auto lower = std::lower_bound(result.begin(), result.end(), startTime,
                [](const Dot& dot, double value) { return dot.x < value; });
            auto upper = std::lower_bound(result.begin(), result.end(), endTime,
                [](const Dot& dot, double value) { return dot.x < value; });

            // Применяем импульс к точкам в диапазоне
            for (auto it = lower; it != upper; ++it) {
                double localX = it->x - startTime;
                it->y += sign * getImpulseValue(form, localX, impulseLength, aVal, bVal);
            }
        }
    }
}
