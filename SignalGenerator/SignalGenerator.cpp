#include "SignalGenerator.h"
#include <cmath>     // Для математических операций

std::vector<Dot> generateSignalFromBits(const std::string& bits,
    conversionMethod method, double dt, double A, double bitDuration, bool polarity) 
{
    std::vector<Dot> series; // Массив точек
    bool lastPolarity = false; // Для метода AMI (следить за последней полярностью)
    int stepsPerBit = static_cast<int>(ceil(bitDuration / dt)); // кол-во шагов на один бит
    series.reserve(stepsPerBit * bits.size());
    int globalStep = 0; // счётчик шагов
    double t = 0.0; // тайм текущей точки на X

    for (char bit : bits) {
        switch (method) {
            case conversionMethod::NRZ: {
                // Non-Return-to-Zero: 0 -> -A, 1 -> +A
                double amplitude = (bit == '1') ? A : -A;
                if (polarity) amplitude = -amplitude;

                for (int i = 0; i < stepsPerBit; ++i) {
                    t = globalStep * dt;
                    series.emplace_back(t, amplitude);
                    ++globalStep;
                }
                break;
            }

            case conversionMethod::MANCH: {
                // Manchester: 0 -> [A, -A], 1 -> [-A, A]
                double firstHalf = (bit == '1') ? -A : A;
                double secondHalf = -firstHalf;

                int halfSteps = stepsPerBit / 2;
                for (int i = 0; i < halfSteps; ++i) {
                    t = globalStep * dt;
                    series.emplace_back(t, firstHalf);
                    ++globalStep;
                }
                for (int i = halfSteps; i < stepsPerBit; ++i) {
                    t = globalStep * dt;
                    series.emplace_back(t, secondHalf);
                    ++globalStep;
                }
                break;
            }

            case conversionMethod::RZ: {
                // Return-to-Zero: 0 -> [0], 1 -> [A, 0]
                double amplitude = (bit == '1') ? A : 0;
                if (polarity && bit == '1') amplitude = -amplitude;

                int halfSteps = stepsPerBit / 2;
                for (int i = 0; i < halfSteps; ++i) {
                    t = globalStep * dt;
                    series.emplace_back(t, amplitude);
                    ++globalStep;
                }
                for (int i = halfSteps; i < stepsPerBit; ++i) {
                    t = globalStep * dt;
                    series.emplace_back(t, 0.0);
                    ++globalStep;
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

                for (int i = 0; i < stepsPerBit; ++i) {
                    t = globalStep * dt;
                    series.emplace_back(t, amplitude);
                    ++globalStep;
                }
                break;
            }
        }
    }
    return series;
}