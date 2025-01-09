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
// Генерация случайного числа типа double в интервале (minVal, maxVal).
double randomDouble(std::mt19937& gen, double minVal, double maxVal)
{
    // Используем равномерное распределение
    std::uniform_real_distribution<double> dist(minVal, maxVal);
    return dist(gen);
}

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

std::vector<Dot> generateNoise(int randSeed, const std::vector<Dot>& signal, double t, double dt, int nu, int dnu,
    noiseForm form, bool polarity, std::vector<double> params)
{
    if (signal.size() <= 1)
    {
        // Если меньше двух точек, нет смысла что-то делать.
        return;
    }

    // Инициализируем псевдо рандомные числа, используя заданный randSeed
    std::mt19937 gen(randSeed);

    // Шаг по оси X (предполагаем равномерный сигнал)
    double z = signal[1].x - signal[0].x;
    if (z <= 0.0)
    {
        // Если точки имеют совпадающие X или убывают, 
        // тоже ничего не делаем
        return;
    }

    // Определяем временной диапазон сигнала: от xMin до xMax
    double xMin = signal.front().x;
    double xMax = signal.back().x;
    // Целое кол-во секунд – пробежимся от floor(xMin) до floor(xMax).
    int startSecond = static_cast<int>(std::floor(xMin));
    int endSecond = static_cast<int>(std::floor(xMax));
    
    // Счётчик знаков при Polarity::ALTERNATE_SIGN
    bool usePlusSign = true;

    // Проходим по каждой секунде
    for (int sec = startSecond; sec <= endSecond; ++sec)
    {
        // Если sec выходит за область сигнала (меньше xMin или больше xMax),
        // теоретически можно пропускать, но обычно при floor(...) это не критично.
        if (sec < xMin || sec > xMax)
            continue;
    
        // Выбираем, сколько началов импульса будет в этой секунде
        double impulsesInThisSecond = randomDouble(gen, nu - dnu, nu + dnu);
        // Округляем до ближайшего целого, чтобы получить "x начал помех"
        int numImpulses = static_cast<int>(std::round(impulsesInThisSecond));
        if (numImpulses < 0) numImpulses = 0; // На всякий случай
    
        // Для каждого из этих началов накладываем импульс
        for (int i = 0; i < numImpulses; ++i)
        {
            // Выбираем случайный момент начала импульса внутри [sec, sec+1)
            double startTime = sec + randomDouble(gen, 0.0, 1.0);
    
            // Случайная длительность из (t-dt, t+dt)
            double impulseLength = randomDouble(gen, t - dt, t + dt);
            if (impulseLength <= 0.0)
                continue; // если вдруг получилось <= 0, пропускаем
    
            // Случайные параметры a и b, если нужно
            double aVal = randomDouble(gen, params.a_mean - params.a_delta,
                params.a_mean + params.a_delta);
            double bVal = randomDouble(gen, params.b_mean - params.b_delta,
                params.b_mean + params.b_delta);
    
            // Выбираем знак, если Polarity::ALTERNATE_SIGN
            double sign = 1.0;
            if (polarity == 1)
            {
                sign = usePlusSign ? 1.0 : -1.0;
                usePlusSign = !usePlusSign;
            }
            else if (polarity == 0)
            {
                sign = 1.0;
            }
    
            // Теперь идём по всем точкам сигнала, которые попадают
            // во временной промежуток [startTime, startTime + impulseLength]
            // и прибавляем соответствующие значения импульса.
            double endTime = startTime + impulseLength;
    
            // Чтобы не проходить по всему сигналу (особенно если он большой),
            // можно найти индексы вектора, которые попадают в нужный диапазон времени.
            // Но для простоты пройдёмся по всем.
            for (size_t j = 0; j < signal.size(); ++j)
            {
                double xCoord = signal[j].x;
                if (xCoord < startTime || xCoord > endTime)
                    continue; // вне диапазона импульса
    
                // localX = сколько прошло времени от начала импульса
                double localX = xCoord - startTime;
                // Считаем значение формы импульса в localX и добавляем к signal[j].y
                double val = getImpulseValue(form, localX, impulseLength, aVal, bVal);
                signal[j].y += sign * val;
            }
        }
    }
    return std::vector<Dot>();
}
