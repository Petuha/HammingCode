#include "SignalIdentificator.h"

std::string bitsFromSignal(const std::vector<Dot>& signal, conversionMethod method, double dt, double A, double bitDuration, bool polarity) {
    std::string bitSequence;
    int samplesPerBit = static_cast<int>(ceil(bitDuration / dt));  // Количество образцов на бит
    int signalSize = static_cast<int>(signal.size());
    bitSequence.reserve(signalSize / samplesPerBit);  // Резервируем память для битов

    for (int i = 0; i < signalSize; i += samplesPerBit) {
        try {
            switch (method) {
                case conversionMethod::NRZ: {
                    int countAboveZero = 0, countBelowZero = 0;
                    for (int j = i; j < i + samplesPerBit; ++j) {
                        bool isAboveZero = (signal[j].y > 0);
                        if ((isAboveZero ^ polarity)) {
                            ++countAboveZero;
                        } else {
                            ++countBelowZero;
                        }
                    }
                    char decodedBit = (countAboveZero > countBelowZero) ? '1' : '0';
                    bitSequence.push_back(decodedBit);
                    break;
                }

                case conversionMethod::MANCH: {
                    bool firstHalfIsHigh = false, secondHalfIsHigh = false;
                    int halfSamples = samplesPerBit / 2;

                    // Первая половина: проверка, высокая ли амплитуда в первой половине
                    for (int j = i; j < i + halfSamples; ++j) {
                        firstHalfIsHigh |= (signal[j].y > 0);
                    }

                    // Вторая половина: проверка, высокая ли амплитуда во второй половине
                    for (int j = i + halfSamples; j < i + samplesPerBit; ++j) {
                        secondHalfIsHigh |= (signal[j].y > 0);
                    }

                    // Декодирование на основе Manchester
                    char decodedBit;
                    if (firstHalfIsHigh && !secondHalfIsHigh) {
                        decodedBit = '0';  // Переход с высокого на низкий для 0
                    } else if (!firstHalfIsHigh && secondHalfIsHigh) {
                        decodedBit = '1';  // Переход с низкого на высокий для 1
                    } else {
                        decodedBit = '0';  // Любой другой случай (например, оба высокие или оба низкие) считаем как 0
                    }
                    bitSequence.push_back(decodedBit);
                    break;
                }

                case conversionMethod::RZ: {
                    bool isOne = false;
                    int halfSamples = samplesPerBit / 2;
                    double threshold = 0.01;  // Порог для амплитуды, чтобы избежать ошибок с нулем

                    // Первая половина: проверка амплитуды для 1 (или 0 для бита 0)
                    for (int j = i; j < i + halfSamples; ++j) {
                        if ((polarity == 0 && signal[j].y > threshold) || (polarity == 1 && signal[j].y < -threshold)) {
                            isOne = true;
                            break;
                        }
                    }

                    bool returnsToZero = true;
                    // Вторая половина: проверка на возврат к нулю с небольшим допуском для погрешностей
                    for (int j = i + halfSamples; j < i + samplesPerBit; ++j) {
                        if (std::abs(signal[j].y) > threshold) {
                            returnsToZero = false;
                            break;
                        }
                    }

                    // Декодирование
                    char decodedBit = '0';  // по умолчанию считаем 0
                    if (isOne && returnsToZero) {
                        decodedBit = '1';
                    }

                    bitSequence.push_back(decodedBit);
                    break;
                }

                case conversionMethod::AMI: {
                    bool lastBitWasPositive = true;  // Начнем с положительной амплитуды для первого '1'
                    bool bitDetected = false;

                    for (int j = i; j < i + samplesPerBit; ++j) {
                        if (signal[j].y > 0) {
                            // Если сигнал положительный и предыдущий бит был отрицательным, ставим '1'
                            if (!lastBitWasPositive) {
                                bitSequence.push_back('1');
                                lastBitWasPositive = true;  // Переход к положительной амплитуде для следующего бита
                            } else {
                                bitSequence.push_back('0');  // Это '0', потому что полярность не изменилась
                            }
                            bitDetected = true;
                        } else if (signal[j].y < 0) {
                            // Если сигнал отрицательный и предыдущий бит был положительным, ставим '1'
                            if (lastBitWasPositive) {
                                bitSequence.push_back('1');
                                lastBitWasPositive = false;  // Переход к отрицательной амплитуде для следующего бита
                            } else {
                                bitSequence.push_back('0');  // Это '0', так как полярность не изменилась
                            }
                            bitDetected = true;
                        } else if (!bitDetected) {
                            // Если сигнал равен нулю и мы не видели бит, ставим '0'
                            bitSequence.push_back('0');
                        }
                    }
                    break;
                }

                default:
                    std::cerr << "Unknown conversion method" << std::endl;
                    return "";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing signal at index " << i << ": " << e.what() << std::endl;
            return ""; // Возвращаем пустую строку при ошибке
        }
    }

    return bitSequence;
}

