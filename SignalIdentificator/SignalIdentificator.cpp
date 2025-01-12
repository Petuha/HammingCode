#include "SignalIdentificator.h"

std::string bitsFromSignal(const std::vector<Dot>& signal, conversionMethod method, double dt, double A, double bitDuration, bool polarity) {
    std::string bitSequence;
    int samplesPerBit = static_cast<int>(ceil(bitDuration / dt));  // Используем ceil для генерации
    int signalSize = static_cast<int>(signal.size());
    bitSequence.reserve(signalSize / samplesPerBit);  // Резервируем память

    for (int i = 0; i < signalSize; i += samplesPerBit) {
        int countAboveZero = 0, countBelowZero = 0;
        int halfSamples = samplesPerBit / 2;

        switch (method) {
            case conversionMethod::NRZ: {
                for (int j = i, cnt = 0; cnt < samplesPerBit; ++j, ++cnt) {
                    if (signal[j].y > 0) {
                        ++countAboveZero;
                    } else {
                        ++countBelowZero;
                    }
                }
                bitSequence.push_back(((countAboveZero > countBelowZero) ^ polarity) ? '1' : '0');
                break;
            }
            case conversionMethod::MANCH: {
                int firstHalfAbove = 0, firstHalfBelow = 0, secondHalfAbove = 0, secondHalfBelow = 0;
                for (int j = i, cnt = 0; cnt < halfSamples; ++j, ++cnt) {
                    if (signal[j].y > 0) {
                        ++firstHalfAbove;
                    } else {
                        ++firstHalfBelow;
                    }
                }
                for (int j = i + halfSamples, cnt = 0; cnt < halfSamples; ++j, ++cnt) {
                    if (signal[j].y > 0) {
                        ++secondHalfAbove;
                    } else {
                        ++secondHalfBelow;
                    }
                }
                bitSequence.push_back((firstHalfBelow > secondHalfBelow) ? '1' : '0');
                break;
            }
            case conversionMethod::RZ: {
                int firstHalfAbove = 0, firstHalfBelow = 0;
                for (int j = i, cnt = 0; cnt < halfSamples; ++j, ++cnt) {
                    if (fabs(signal[j].y) > A / 2) {
                        ++firstHalfAbove;
                    } else {
                        ++firstHalfBelow;
                    }
                }
                bitSequence.push_back((firstHalfAbove > firstHalfBelow) ? '1' : '0');
                break;
            }
            case conversionMethod::AMI: {
                for (int j = i, cnt = 0; cnt < samplesPerBit; ++j, ++cnt) {
                    if (fabs(signal[j].y) > A / 2) {
                        ++countAboveZero;
                    } else {
                        ++countBelowZero;
                    }
                }
                bitSequence.push_back((countAboveZero > countBelowZero) ? '1' : '0');
                break;
            }
        }
    }
    return bitSequence;
}

