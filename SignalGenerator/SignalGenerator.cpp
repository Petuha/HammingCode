#include "SignalGenerator.h"
#include <cmath>

std::vector<Dot> generateSignalFromBits(const std::string& bits,
    conversionMethod method, double dt, double A, double bitDuration, bool polarity) 
{
    std::vector<Dot> series; // Массив точек
    bool lastPolarity = false; // Для метода AMI
    int stepsPerBit = static_cast<int>(bitDuration / dt); // Количество шагов на 1 бит без округления
    double t = 0.0; // Время текущей точки

    for (char bit : bits) {
        double amplitude = 0.0;
        double lastAmplitude = 0.0; // Для запоминания амплитуды предыдущей точки
        switch (method) {
            case conversionMethod::NRZ: {
                // Для NRZ: 0 -> -A, 1 -> +A
                amplitude = (bit == '1') ? A : -A;
                if (polarity) amplitude = -amplitude;

                // Для каждого шага битов амплитуда постоянна
                for (int i = 0; i < stepsPerBit; ++i) {
                    series.emplace_back(t, amplitude);
                    lastAmplitude = amplitude; // Запоминаем амплитуду
                    t += dt; // Увеличиваем время
                }

                // Добавляем последнюю точку с текущей амплитудой
                series.emplace_back(t, lastAmplitude); 
                break;
            }

            case conversionMethod::MANCH: {
                double firstHalf = (bit == '1') ? -A : A;
                double secondHalf = -firstHalf;
                int halfSteps = stepsPerBit / 2;

                // Первая половина битов
                for (int i = 0; i < halfSteps; ++i) {
                    series.emplace_back(t, firstHalf);
                    lastAmplitude = firstHalf; // Запоминаем амплитуду
                    t += dt;
                }

                // Добавляем последнюю точку 
                series.emplace_back(t, lastAmplitude);

                // Вторая половина битов
                for (int i = halfSteps; i < stepsPerBit; ++i) {
                    series.emplace_back(t, secondHalf);
                    lastAmplitude = secondHalf; // Запоминаем амплитуду
                    t += dt;
                }

                // Добавляем последнюю точку 
                series.emplace_back(t, lastAmplitude); 
                break;
            }

            case conversionMethod::RZ: {
                amplitude = (bit == '1') ? A : 0;
                if (polarity && bit == '1') amplitude = -amplitude;

                int halfSteps = stepsPerBit / 2;

                // Первая половина битов (амплитуда A или 0)
                for (int i = 0; i < halfSteps; ++i) {
                    series.emplace_back(t, amplitude);
                    lastAmplitude = amplitude; // Запоминаем амплитуду
                    t += dt; // Увеличиваем время
                }

                // Добавляем последнюю точку 
                series.emplace_back(t, lastAmplitude); 

                // Вторая половина битов (амплитуда 0)
                for (int i = halfSteps; i < stepsPerBit; ++i) {
                    series.emplace_back(t, 0.0);
                    lastAmplitude = 0.0; // Запоминаем амплитуду
                    t += dt; // Увеличиваем время
                }

                // Добавляем последнюю точку 
                series.emplace_back(t, lastAmplitude); 
                break;
            }

            case conversionMethod::AMI: {
                amplitude = 0.0;
                if (bit == '1') {
                    amplitude = lastPolarity ? -A : A;
                    lastPolarity = !lastPolarity;
                }

                // Для каждого шага добавляем точки с чередующейся амплитудой
                for (int i = 0; i < stepsPerBit; ++i) {
                    series.emplace_back(t, amplitude);
                    lastAmplitude = amplitude; // Запоминаем амплитуду
                    t += dt; // Увеличиваем время
                }

                // Добавляем последнюю точку 
                series.emplace_back(t, lastAmplitude); 
                break;
            }
        }
    }
    return series;
}