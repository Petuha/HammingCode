#pragma once
#include <vector>
#include <string>

// Точка на графике сигнала
struct Dot {
    double x; // Время
    double y; // Амплитуда

    // Конструктор для инициализации x и y
    Dot(double x, double y) : x(x), y(y) {}
};

// Методы преобразования битовой последовательности в сигнал
enum class conversionMethod {
    NRZ,   // Non-Return-to-Zero
    MANCH, // Манчестерский код
    RZ,    // Return-to-Zero
    AMI    // Alternate Mark Inversion
};

// Функция для генерации сигнала из бит
std::vector<Dot> generateSignalFromBits(const std::string& bits,
    conversionMethod method, double dt, double A, int dotsPerBit, bool polarity);