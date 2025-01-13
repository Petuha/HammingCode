#include "SignalIdentificator.h"

std::string bitsFromSignalNRZ(const std::vector<Dot>& signal, double dt, double A, double bitDuration, bool polarity) {
  std::string bitSequence;
  int samplesPerBit = static_cast<int>(ceil(bitDuration / dt));
  int signalSize = static_cast<int>(signal.size());
  bitSequence.reserve(signalSize / samplesPerBit);
  int one, zero;
  for (int i = 0; i < signalSize; i += samplesPerBit) {
    one = zero = 0;
    for (int j = i, cnt = 0; cnt < samplesPerBit; ++cnt) {
      if (!polarity) {
        if (signal[j].y > A / 2) ++one;
        else ++zero;
      }
      else {
        if (signal[j].y < -A / 2) ++one;
        else ++zero;
      }
    }
    bitSequence.push_back(one > zero ? '1' : '0');
  }
  return bitSequence;
}

std::string bitsFromSignalMANCH(const std::vector<Dot>& signal, double dt, double A, double bitDuration, bool polarity) {
  std::string bitSequence;
  int samplesPerBit = static_cast<int>(ceil(bitDuration / dt));
  int signalSize = static_cast<int>(signal.size());
  bitSequence.reserve(signalSize / samplesPerBit);
  int halfSamples = samplesPerBit / 2;
  int one1, zero1, one2, zero2;
  for (int i = 0; i < signalSize; i += samplesPerBit) {
    one1 = zero1 = one2 = zero2 = 0;
    for (int j = i, cnt = 0; cnt < halfSamples; ++cnt) {
      if (!polarity) {
        if (signal[j].y < -A / 2) ++one1;
        else ++zero1;
      }
      else {
        if (signal[j].y > A / 2) ++one1;
        else ++zero1;
      }
    }
    for (int j = i + halfSamples, cnt = halfSamples; cnt < samplesPerBit; ++cnt) {
      if (!polarity) {
        if (signal[j].y > A / 2) ++one2;
        else ++zero2;
      }
      else {
        if (signal[j].y < -A / 2) ++one2;
        else ++zero2;
      }
    }
    bitSequence.push_back(one1 + one2 > zero1 + zero2 ? '1' : '0');
  }
  return bitSequence;
}

std::string bitsFromSignalRZ(const std::vector<Dot>& signal, double dt, double A, double bitDuration, bool polarity) {
  std::string bitSequence;
  int samplesPerBit = static_cast<int>(ceil(bitDuration / dt));
  int signalSize = static_cast<int>(signal.size());
  bitSequence.reserve(signalSize / samplesPerBit);
  int halfSamples = samplesPerBit / 2;
  int one, zero;
  for (int i = 0; i < signalSize; i += samplesPerBit) {
    one = zero = 0;
    for (int j = i, cnt = 0; cnt < halfSamples; ++cnt) {
      if (!polarity) {
        if (signal[j].y > A / 2) ++one;
        else ++zero;
      }
      else {
        if (signal[j].y < -A / 2) ++one;
        else ++zero;
      }
    }
    // Вторую половину анализировать смысла нет, т.к. она одинаковая для 0 и 1
    bitSequence.push_back(one > zero ? '1' : '0');
  }
  return bitSequence;
}

std::string bitsFromSignalAMI(const std::vector<Dot>& signal, double dt, double A, double bitDuration, bool polarity) {
  std::string bitSequence;
  int samplesPerBit = static_cast<int>(ceil(bitDuration / dt));
  int signalSize = static_cast<int>(signal.size());
  bitSequence.reserve(signalSize / samplesPerBit);
  int oneNeg, onePos, zero;
  for (int i = 0; i < signalSize; i += samplesPerBit) {
    oneNeg = onePos = zero = 0;
    for (int j = i, cnt = 0; cnt < samplesPerBit; ++cnt) {
      if (signal[j].y > A / 2) ++onePos;
      else if (signal[j].y < -A / 2) ++oneNeg;
      else ++zero;
    }
    bitSequence.push_back(onePos > zero || oneNeg > zero ? '1' : '0');
  }
  return bitSequence;
}

std::string bitsFromSignal(const std::vector<Dot>& signal, conversionMethod method, double dt, double A, double bitDuration, bool polarity) {
  switch (method) {
  case conversionMethod::NRZ:
    return bitsFromSignalNRZ(signal, dt, A, bitDuration, polarity);
  case conversionMethod::MANCH:
    return bitsFromSignalMANCH(signal, dt, A, bitDuration, polarity);
  case conversionMethod::RZ:
    return bitsFromSignalRZ(signal, dt, A, bitDuration, polarity);
  case conversionMethod::AMI:
    return bitsFromSignalAMI(signal, dt, A, bitDuration, polarity);
  default:
    return "";
  }
  return "";
}

