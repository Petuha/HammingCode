#include "SignalIdentificator.h"

std::string bitsFromSignal(const std::vector<Dot>& signal, conversionMethod method, double dt, double A, double bitDuration, bool polarity) {
    std::string bitSequence;
    size_t samplesPerBit = static_cast<size_t>(bitDuration / dt);

    for (size_t i = 0; i < signal.size(); i += samplesPerBit) {
        double sumAmplitude = 0.0;
        for (size_t j = i; j < i + samplesPerBit && j < signal.size(); ++j) {
            sumAmplitude += signal[j].y;
        }
        double averageAmplitude = sumAmplitude / samplesPerBit;

        switch (method) {
            case conversionMethod::NRZ:
                bitSequence += (averageAmplitude > A / 2) ^ polarity ? '1' : '0';
                break;
            case conversionMethod::MANCH:
                bitSequence += (averageAmplitude > A / 2) ? '1' : '0';
                break;
            case conversionMethod::RZ:
                bitSequence += ((i / samplesPerBit) % 2 == 0) ^ polarity ? '1' : '0';
                break;
            case conversionMethod::AMI:
                if (averageAmplitude > A / 2) {
                    polarity = !polarity;
                    bitSequence += polarity ? '1' : '1';
                } else {
                    bitSequence += '0';
                }
                break;
            default:
                throw std::invalid_argument("Unknown conversion method");
        }
    }
    return bitSequence;
}
