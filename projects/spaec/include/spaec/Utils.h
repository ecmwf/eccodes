#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace spaec
{

enum class ReferenceType
{
    Ref,
    NoRef
};

constexpr size_t rosLength = 5;

// Largest reference sample interval (in blocks), CCSDS 121.0-B-3, 4.3
constexpr size_t maxRsiSize = 4096;

// Checks the coding parameters (CCSDS 121.0-B-3, 3.1.6 and 4.3) and throws
// std::invalid_argument with a helpful message if they are not supported.
// maxBpv is the number of bits of the sample type used by the caller.
inline void validateCcsdsParameters(size_t bpv, size_t blockSize, size_t rsiSize, size_t maxBpv = 32)
{
    if (bpv == 0 || bpv > 32 || bpv > maxBpv) {
        throw std::invalid_argument("CCSDS: unsupported number of bits per value " + std::to_string(bpv) + " (must be between 1 and " + std::to_string(maxBpv < 32 ? maxBpv : 32) + ")");
    }
    if (blockSize != 8 && blockSize != 16 && blockSize != 32 && blockSize != 64) {
        throw std::invalid_argument("CCSDS: unsupported block size " + std::to_string(blockSize) + " (must be 8, 16, 32 or 64)");
    }
    if (rsiSize == 0 || rsiSize > maxRsiSize) {
        throw std::invalid_argument("CCSDS: unsupported reference sample interval " + std::to_string(rsiSize) + " (must be between 1 and " + std::to_string(maxRsiSize) + " blocks)");
    }
}

}  // namespace spaec
