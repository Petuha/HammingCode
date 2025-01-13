#include "SignalIdentificator.h"

std::string bitsFromSignal(const std::vector<Dot>& signal, conversionMethod method, double dt, double A, double bitDuration, bool polarity) {
    std::string bitSequence;
    int samplesPerBit = static_cast<int>(ceil(bitDuration / dt));
    int signalSize = static_cast<int>(signal.size());
    bitSequence.reserve(signalSize / samplesPerBit);

    for (int i = 0; i < signalSize; i += samplesPerBit) {
        int countAboveZero = 0, countBelowZero = 0;
        int halfSamples = samplesPerBit / 2;

        switch (method) {
            case conversionMethod::NRZ: {
                for (int j = i, cnt = 0; cnt < samplesPerBit; ++j, ++cnt) {
                    if ((signal[j].y > 0) ^ polarity) {
                        ++countAboveZero;
                    } else {
                        ++countBelowZero;
                    }
                }
                bitSequence.push_back((countAboveZero > countBelowZero) ? '1' : '0');
                break;
            }
            case conversionMethod::MANCH: {
                int firstHalfAbove = 0, firstHalfBelow = 0, secondHalfAbove = 0, secondHalfBelow = 0;
                for (int j = i, cnt = 0; cnt < halfSamples; ++j, ++cnt) {
                    if ((signal[j].y > 0) ^ polarity) {
                        ++firstHalfAbove;
                    } else {
                        ++firstHalfBelow;
                    }
                }
                for (int j = i + halfSamples, cnt = halfSamples; cnt < samplesPerBit; ++j, ++cnt) {
                    if ((signal[j].y > 0) ^ polarity) {
                        ++secondHalfAbove;
                    } else {
                        ++secondHalfBelow;
                    }
                }
                bitSequence.push_back(((firstHalfBelow > secondHalfBelow) ^ polarity) ? '1' : '0');
                break;
            }
            case conversionMethod::RZ: {
                int countFirstHalf = 0, countSecondHalf = 0;
                for (int j = i, cnt = 0; cnt < halfSamples; ++j, ++cnt) {
                    if ((signal[j].y > A / 2) ^ polarity) {
                        ++countFirstHalf;
                    }
                }
                for (int j = i + halfSamples, cnt = halfSamples; cnt < samplesPerBit; ++j, ++cnt) {
                    if ((signal[j].y > A / 2) ^ polarity) {
                        ++countSecondHalf;
                    }
                }
                bitSequence.push_back((countFirstHalf > countSecondHalf) ? '1' : '0');
                break;
            }
            case conversionMethod::AMI: {
                for (int j = i, cnt = 0; cnt < samplesPerBit; ++j, ++cnt) {
                    if ((signal[j].y > 0) ^ polarity) {
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

