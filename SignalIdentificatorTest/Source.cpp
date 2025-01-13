#include "../SignalGenerator/SignalGenerator.h"
#include "../SignalIdentificator/SignalIdentificator.h"

#include <iostream>
#include <random>
std::string generateRandomBits(int length) {
    std::string bits;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    for (int i = 0; i < length; ++i) {
        bits += (dist(gen) == 1) ? '1' : '0';
    }
    return bits;
}

double getRandomDouble(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(min, max);
    return dist(gen);
}

std::vector<std::string> names = { "NRZ", "MANCH", "RZ", "AMI" };

void testConversionMethod(conversionMethod method, double dt, double A, double bitDuration, bool polarity, bool& testPassed, std::string& failedParams) {
    int testLength = 20;
    std::string originalBits = generateRandomBits(testLength);

    std::vector<Dot> signal = generateSignalFromBits(originalBits, method, dt, A, bitDuration, polarity);
    std::string decodedBits = bitsFromSignal(signal, method, dt, A, bitDuration, polarity);

    if (originalBits == decodedBits) {
        testPassed = true;
    }
    else {
        testPassed = false;
        failedParams = "Method " + names[static_cast<int>(method)] +
            ": dt = " + std::to_string(dt) +
            ", A = " + std::to_string(A) +
            ", bitDuration = " + std::to_string(bitDuration) +
            ", polarity = " + std::to_string(polarity) +
            ", originalBits = " + originalBits +
            ", decodedBits = " + decodedBits;
    }
}

int main() {
    std::cout << "Starting tests..." << std::endl;

    // Переменная для отслеживания общего статуса
    bool allTestsPassed = true;
    std::vector<std::string> failedTestsParams;

    // Массив методов для тестирования
    std::vector<conversionMethod> methods = {
            conversionMethod::NRZ,
            conversionMethod::MANCH,
            conversionMethod::RZ,
            conversionMethod::AMI
    };

    // Количество тестов
    int numTests = 100000;
    numTests *= 2;

    for (int i = 0; i < numTests; ++i) {
        for (const auto& method : methods) {
            double dt = getRandomDouble(0.5, 10.0);
            double A = getRandomDouble(0.5, 10.0);
            double bitDuration = getRandomDouble(dt, 11.0);
            bool polarity = i % 2;

            bool testPassed = false;
            std::string failedParams;

            testConversionMethod(method, dt, A, bitDuration, polarity, testPassed, failedParams);
            if (!testPassed) {
                allTestsPassed = false;
                failedTestsParams.push_back(failedParams);
            }
        }
    }

    // Вывод результатов
    if (!allTestsPassed) {
        std::cout << "Some tests failed. Failed test parameters:" << std::endl;
        for (const auto& failedTest : failedTestsParams) {
            std::cout << failedTest << std::endl;
        }
    }

    std::cout << "All tests completed." << std::endl;
    return 0;
}
