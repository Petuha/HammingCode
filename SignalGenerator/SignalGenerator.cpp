#include "SignalGenerator.h"
#include <cmath>

std::vector<Dot> generateSignalFromBits(const std::string& bits,
    conversionMethod method, double dt, double A, double bitDuration, bool polarity) 
{
    std::vector<Dot> series; // Массив точек
    bool lastPolarity = false; // Для метода AMI (следить за последней полярностью)
    int stepsPerBit = static_cast<int>(ceil(bitDuration / dt)); // кол-во шагов на один бит
    int globalStep = 0; // счётчик шагов
    double t = 0.0; // тайм текущей точки на X
    double prevDot = -1.0; // предыдущая точка

    for (char bit : bits) {
        switch (method) {
            case conversionMethod::NRZ: {
                double amplitude = (bit == '1') ? A : -A;
                if (polarity) amplitude = -amplitude;

                for (int i = 0; i <= stepsPerBit; ++i, ++globalStep) { // <= stepsPerBit для добавления последней точки
                    t = globalStep * dt;
                    // Проверка на уникальность точки
                    if (t != prevDot) {
                        series.emplace_back(t, amplitude);
                        prevDot = t;
                    }
                }
                --globalStep; // Возвращаемся на 1 шаг назад
                break;
            }

            case conversionMethod::MANCH: {
                double firstHalf = (bit == '1') ? -A : A;
                double secondHalf = -firstHalf;

                int halfSteps = stepsPerBit / 2;
                for (int i = 0; i < halfSteps; ++i, ++globalStep) {
                    t = globalStep * dt;
                    if (t != prevDot) {
                        series.emplace_back(t, firstHalf);
                        prevDot = t;
                    }
                }
                for (int i = halfSteps; i <= stepsPerBit; ++i, ++globalStep) {
                    t = globalStep * dt;
                    if (t != prevDot) {
                        series.emplace_back(t, secondHalf);
                        prevDot = t;
                    }
                }
                --globalStep;
                break;
            }

            case conversionMethod::RZ: {
                double amplitude = (bit == '1') ? A : 0;
                if (bit == '1' && polarity) amplitude = -amplitude;

                int halfSteps = stepsPerBit / 2;
                for (int i = 0; i < halfSteps; ++i, ++globalStep) {
                    t = globalStep * dt;
                    if (t != prevDot) {
                        series.emplace_back(t, amplitude);
                        prevDot = t;
                    }
                }
                for (int i = halfSteps; i <= stepsPerBit; ++i, ++globalStep) {
                    t = globalStep * dt;
                    if (t != prevDot) {
                        series.emplace_back(t, 0.0);
                        prevDot = t;
                    }
                }
                --globalStep;
                break;
            }

            case conversionMethod::AMI: {
                double amplitude = 0.0;
                if (bit == '1') {
                    amplitude = lastPolarity ? -A : A;
                    lastPolarity = !lastPolarity;
                }

                for (int i = 0; i <= stepsPerBit; ++i, ++globalStep) {
                    t = globalStep * dt;
                    if (t != prevDot) {
                        series.emplace_back(t, amplitude);
                        prevDot = t;
                    }
                }
                --globalStep;
                break;
            }
        }
    }
    return series;
}