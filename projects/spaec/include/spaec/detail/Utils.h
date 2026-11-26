/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace spaec::detail
{

enum class ReferenceType
{
    Ref,
    NoRef
};

constexpr size_t rosLength = 5;

// Largest reference sample interval (in blocks), CCSDS 121.0-B-3, 4.3
constexpr size_t maxRsiSize = 4096;

// The preprocessing of the samples (CCSDS 121.0-B-3, 4)
enum class Prediction
{
    UnitDelay,  // preprocessor with the unit-delay predictor (4.2.5) and reference samples (4.2.6)
    Bypass,     // preprocessor with the bypass predictor (4.2.3): predicted values are 0, the mapper
                // remains; no reference samples
    None        // no preprocessor (4.1.1): the samples are coded as they are; no reference samples
};

// The selection of the parameter k of the split-sample options (CCSDS 121.0-B-3, 3.3), see
// encoder::EncoderBase
enum class SplitSelector
{
    Search,   // the k of minimum block size, searched from the k of the previous block (as libaec)
    Estimate  // k estimated from the sum of the block (Kiely 2004)
};

struct Preprocessing
{
    Prediction prediction = Prediction::UnitDelay;
    // The samples are n-bit two's complement numbers (4.4: xmin = -2^(n-1), xmax = 2^(n-1) - 1).
    // They are passed in the unsigned sample type of the coder: the encoder codes their n low bits,
    // the decoder returns them sign-extended to the sample type.
    bool signedSamples = false;

    bool isDefault() const { return prediction == Prediction::UnitDelay && !signedSamples; }
};

// Checks the preprocessing and throws std::invalid_argument if it is not supported
inline void validatePreprocessing(const Preprocessing& preprocessing)
{
    if (preprocessing.prediction == Prediction::None && preprocessing.signedSamples) {
        throw std::invalid_argument("CCSDS: signed samples need a preprocessor: UnitDelay or Bypass (CCSDS 121.0-B-3, 6.3.3.5.2: without preprocessor the data sense is positive)");
    }
}

// Checks the coding parameters (CCSDS 121.0-B-3, 3.1.6 and 4.3) and throws
// std::invalid_argument with a helpful message if they are not supported.
// maxBpv is the number of bits of the sample type used by the caller.
inline void validateCcsdsParameters(size_t bpv, size_t blockSize, size_t rsiSize, size_t maxBpv = 32)
{
    if (bpv == 0 || bpv > 32 || bpv > maxBpv) {
        throw std::invalid_argument("CCSDS: unsupported number of bits per value " + std::to_string(bpv) + " (must be between 1 and " + std::to_string(maxBpv < 32 ? maxBpv : 32) +
                                    (maxBpv < 32 ? " for samples of " + std::to_string(maxBpv) + " bits: use a wider sample type)" : ")"));
    }
    if (blockSize != 8 && blockSize != 16 && blockSize != 32 && blockSize != 64) {
        throw std::invalid_argument("CCSDS: unsupported block size " + std::to_string(blockSize) + " (must be 8, 16, 32 or 64)");
    }
    if (rsiSize == 0 || rsiSize > maxRsiSize) {
        throw std::invalid_argument("CCSDS: unsupported reference sample interval " + std::to_string(rsiSize) + " (must be between 1 and " + std::to_string(maxRsiSize) + " blocks)");
    }
}

}  // namespace spaec::detail
