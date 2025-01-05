#include "SignalGenerator.h"
#include <stdexcept> // Для обработки исключений
#include <cmath>     // Для математических операций

std::vector<Dot> generateSignalFromBits(const std::string& bits,
    conversionMethod method, double dt, double A, double bitDuration, bool polarity) 
{
    std::vector<Dot> series; // Массив точек
    double time_start = 0.0; // Текущее время
    bool lastPolarity = false; // Для метода AMI (следить за последней полярностью)
    int steps = static_cast<int>(bitDuration / dt); // кол-во шагов

    for (char bit : bits) {
        double t = 0.0;

        switch (method) {
            case conversionMethod::NRZ: {
                // Non-Return-to-Zero: 0 -> -A, 1 -> +A
                double amplitude = (bit == '1') ? A : -A;
                if (polarity) amplitude = -amplitude;

                for (int i = 0; i < steps; ++i) {
                    t = time_start + i * dt;
                    series.emplace_back(t, amplitude);
                }
                break;
            }

            case conversionMethod::MANCH: {
                // Manchester: 0 -> [A, -A], 1 -> [-A, A]
                double firstHalf = (bit == '1') ? -A : A;
                double secondHalf = -firstHalf;

                int halfSteps = steps / 2;
                for (int i = 0; i < halfSteps; ++i) {
                    t = time_start + i * dt;
                    series.emplace_back(t, firstHalf);
                }
                for (int i = halfSteps; i < steps; ++i) {
                    t = time_start + i * dt;
                    series.emplace_back(t, secondHalf);
                }
                break;
            }

            case conversionMethod::RZ: {
                // Return-to-Zero: 0 -> [0], 1 -> [A, 0]
                double amplitude = (bit == '1') ? A : 0;
                if (polarity && bit == '1') amplitude = -amplitude;

                int halfSteps = steps / 2;
                for (int i = 0; i < halfSteps; ++i) {
                    t = time_start + i * dt;
                    series.emplace_back(t, amplitude);
                }
                for (int i = halfSteps; i < steps; ++i) {
                    t = time_start + i * dt;
                    series.emplace_back(t, 0.0);
                }
                break;
            }

            case conversionMethod::AMI: {
                // Alternate Mark Inversion: 0 -> 0, 1 -> alternating +A and -A
                double amplitude = 0.0;
                if (bit == '1') {
                    amplitude = lastPolarity ? -A : A;
                    lastPolarity = !lastPolarity;
                }

                for (int i = 0; i < steps; ++i) {
                    t = time_start + i * dt;
                    series.emplace_back(t, amplitude);
                }
                break;
            }
        }
        time_start += bitDuration; // Увеличиваем текущее время
    }
    return series;
}