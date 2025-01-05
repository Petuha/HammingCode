#include "SignalGenerator.h"
#include <stdexcept> // Для обработки исключений
#include <cmath>     // Для математических операций

std::vector<Dot> generateSignalFromBits(const std::string& bits,
    conversionMethod method, double dt, double A, double bitDuration, bool polarity)
{
    std::vector<Dot> series; // Массив точек
    double time = 0.0;       // Текущее время
    bool lastPolarity = false; // Для метода AMI (следить за последней полярностью)

    for (char bit : bits) {
        if (bit != '0' && bit != '1') {
            throw std::invalid_argument("Invalid bit sequence. Only '0' and '1' are allowed.");
        }

        switch (method) {
            case conversionMethod::NRZ: {
                // Non-Return-to-Zero: 0 -> -A, 1 -> +A
                double amplitude = (bit == '1') ? A : -A;
                if (polarity) amplitude = -amplitude;

                for (double t = 0; t < bitDuration; t += dt) {
                    series.emplace_back(time + t, amplitude);
                }
                break;
            }

            case conversionMethod::MANCH: {
                // Manchester: 0 -> [A, -A], 1 -> [-A, A]
                double firstHalf = (bit == '1') ? -A : A;
                double secondHalf = -firstHalf;

                for (double t = 0; t < bitDuration / 2; t += dt) {
                    series.emplace_back(time + t, firstHalf);
                }
                for (double t = bitDuration / 2; t < bitDuration; t += dt) {
                    series.emplace_back(time + t, secondHalf);
                }
                break;
            }

            case conversionMethod::RZ: {
                // Return-to-Zero: 0 -> [0], 1 -> [A, 0]
                double amplitude = (bit == '1') ? A : 0;
                if (polarity && bit == '1') amplitude = -amplitude;

                for (double t = 0; t < bitDuration * 0.5; t += dt) {
                    series.emplace_back(time + t, amplitude);
                }
                for (double t = bitDuration * 0.5; t < bitDuration; t += dt) {
                    series.emplace_back(time + t, 0);
                }
                break;
            }

            case conversionMethod::AMI: {
                // Alternate Mark Inversion: 0 -> 0, 1 -> alternating +A and -A
                double amplitude = 0;
                if (bit == '1') {
                    amplitude = lastPolarity ? -A : A;
                    lastPolarity = !lastPolarity;
                }

                for (double t = 0; t < bitDuration; t += dt) {
                    series.emplace_back(time + t, amplitude);
                }
                break;
            }

            default:
                throw std::invalid_argument("Unsupported conversion method.");
        }

        time += bitDuration; // Увеличиваем текущее время
    }

    return series;
}