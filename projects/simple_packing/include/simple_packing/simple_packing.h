/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

// simple_packing: GRIB simple packing of floating-point values into integers of bitsPerValue bits,
//     value * 10^D = R + X * 2^E      (R: reference value, E, D: binary and decimal scale factor)
//
//     #include "simple_packing/simple_packing.h"
//
//     sp::Scaling scaling = sp::scaling(values, sp::Method::bits(16));
//     std::vector<uint8_t> packed = sp::pack(values, scaling);
//     std::vector<double> back = sp::unpack<double>(packed, values.size(), scaling);
//
// A Method says what is given and what is chosen:
//     sp::Method::bits(16)                 16 bits per value, the scale factors for them
//     sp::Method::bestPrecision(16)        16 bits per value, the decimal scale factor that uses them best
//     sp::Method::decimals(2)              2 decimal digits, as many bits as they need
//     sp::Method::bitsAndDecimals(16, 2)   both given, the binary scale factor is chosen
//
// The packed data hold no parameters: unpack needs the Scaling and the number of values.
// Errors are reported by exceptions: std::invalid_argument for invalid parameters and packed data
// that are too short, std::runtime_error for values that cannot be packed (NaN, infinite, a range
// too large for the method; std::underflow_error for a range too small for any binary scale
// factor), std::length_error for an output buffer that is too small.
// Everything in simple_packing/detail is the implementation.

#pragma once

#include <cstddef>
#include <limits>
#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "simple_packing/version.h"
#include "simple_packing/detail/optimizer/BPVOptimizer.h"
#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/optimizer/BinOptimizer.h"
#include "simple_packing/detail/optimizer/DecOptimizer.h"
#include "simple_packing/detail/simple/Binary.h"

namespace sp
{

// The packing parameters, usually from sp::scaling. bitsPerValue 0 is a constant field: every value
// is the reference value and there are no packed data. A Scaling whose bitsPerValue is not set
// (-1) is rejected by pack, unpack and packedSize.
struct Scaling
{
    double referenceValue = 0;
    int binaryScaleFactor = 0;
    int decimalScaleFactor = 0;
    int bitsPerValue = -1;
};

// The format in which the reference value is stored: it is chosen exactly representable
enum class ReferenceFormat
{
    IEEE32,  // GRIB edition 2
    IBM32    // GRIB edition 1
};

// How the scaling of a field is chosen (see the top of this file)
class Method
{
public:
    static Method bits(int bitsPerValue) { return Method(Kind::Bits, bitsPerValue, 0); }
    static Method bestPrecision(int bitsPerValue) { return Method(Kind::BestPrecision, bitsPerValue, 0); }
    static Method decimals(int decimalScaleFactor) { return Method(Kind::Decimals, 0, decimalScaleFactor); }
    static Method bitsAndDecimals(int bitsPerValue, int decimalScaleFactor) { return Method(Kind::BitsAndDecimals, bitsPerValue, decimalScaleFactor); }

    // The format of the reference value (default: IEEE32). Returns a modified copy.
    Method referenceFormat(ReferenceFormat format) const
    {
        Method m = *this;
        m.referenceFormat_ = format;
        return m;
    }

    // The limits of GRIBEX for GRIB edition 1: the binary scale factor is limited as in GRIBEX
    // (magnitude 99 to 127, depending on the method); the decimal scale factor is not limited.
    // Returns a modified copy.
    Method gribex(bool on = true) const
    {
        Method m = *this;
        m.gribex_ = on;
        return m;
    }

private:
    enum class Kind
    {
        Bits,
        BestPrecision,
        Decimals,
        BitsAndDecimals
    };

    Method(Kind kind, int bitsPerValue, int decimalScaleFactor) : kind_(kind), bitsPerValue_(bitsPerValue), decimalScaleFactor_(decimalScaleFactor) {}

    Kind kind_;
    int bitsPerValue_;
    int decimalScaleFactor_;
    ReferenceFormat referenceFormat_ = ReferenceFormat::IEEE32;
    bool gribex_ = false;

    friend Scaling scaling(double min, double max, const Method& method);
};

// The smallest and the largest value. Both are NaN if a value is NaN or infinite.
template <typename Value>
struct Range
{
    Value min;
    Value max;
};

template <typename Value>
Range<Value> range(const Value* values, size_t count)
{
    static_assert(std::is_floating_point<Value>::value, "simple packing packs floating-point values");
    const auto minmax = detail::sp_minmax(values, count);
    return { minmax.first, minmax.second };
}

template <typename Value>
Range<Value> range(const std::vector<Value>& values)
{
    return range(values.data(), values.size());
}

// The scaling for values between min and max (a constant field if they are equal)
inline Scaling scaling(double min, double max, const Method& method)
{
    std::unique_ptr<detail::Optimizer> optimizer;
    switch (method.kind_) {
        case Method::Kind::Bits:
            optimizer = std::make_unique<detail::DecOptimizer>(method.bitsPerValue_, method.gribex_ ? 99 : 127);
            break;
        case Method::Kind::BestPrecision:
            optimizer = std::make_unique<detail::BinDecOptimizer>(method.gribex_ ? 1 : 0, 1, method.bitsPerValue_);
            break;
        case Method::Kind::Decimals:
            optimizer = std::make_unique<detail::BPVOptimizer>(method.decimalScaleFactor_);
            break;
        case Method::Kind::BitsAndDecimals:
            optimizer = std::make_unique<detail::BinOptimizer>(method.decimalScaleFactor_, method.bitsPerValue_);
            break;
    }
    optimizer->referenceFloatType(method.referenceFormat_ == ReferenceFormat::IBM32 ? detail::FloatType::IBM32 : detail::FloatType::IEEE32);
    detail::SimplePackingBinary<double> packer(std::move(optimizer));
    const detail::Parameters p = std::get<0>(packer.getParameters(min, max));
    Scaling s;
    s.referenceValue = p.referenceValue();
    s.binaryScaleFactor = static_cast<int>(p.binaryScaleFactor());
    s.decimalScaleFactor = static_cast<int>(p.decimalScaleFactor());
    s.bitsPerValue = p.isConstantField() ? 0 : static_cast<int>(p.bitsPerValue());
    return s;
}

// The scaling for a field
template <typename Value>
Scaling scaling(const Value* values, size_t count, const Method& method)
{
    const Range<Value> r = range(values, count);
    return scaling(static_cast<double>(r.min), static_cast<double>(r.max), method);
}

template <typename Value>
Scaling scaling(const std::vector<Value>& values, const Method& method)
{
    return scaling(values.data(), values.size(), method);
}

// The size in bytes of count packed values
inline size_t packedSize(size_t count, const Scaling& scaling)
{
    if (scaling.bitsPerValue < 0 || scaling.bitsPerValue > 64) {
        throw std::invalid_argument("Simple packing: bits per value " + std::to_string(scaling.bitsPerValue) +
                                    " is out of range (0 to 64 for unpacking, 1 to 63 for packing, 0: a constant field; a Scaling must be filled in, e.g. by sp::scaling)");
    }
    const size_t bits = static_cast<size_t>(scaling.bitsPerValue);
    if (bits != 0 && count > (SIZE_MAX - 7) / bits) {
        throw std::length_error("Simple packing: the size of " + std::to_string(count) + " packed values does not fit in size_t");
    }
    return (count * bits + 7) / 8;
}

namespace detail
{

// Throws std::invalid_argument if the smallest or the largest code of the scaling does not decode to
// a finite Value (as unpack decodes): pack would accept values whose codes unpack to infinity, e.g.
// the code 512 of FLT_MAX with 10 bits and E = 119 (512 * 2^119 = 2^128)
template <typename Value>
void checkDecodable(const Scaling& s)
{
    const double bscale = sp_power<Value>(s.binaryScaleFactor, 2);
    const double dscale = sp_power<Value>(-s.decimalScaleFactor, 10);
    const double largestCode = s.bitsPerValue > 0 ? std::ldexp(1.0, s.bitsPerValue) - 1.0 : 0.0;
    const double smallest = (0.0 * bscale + s.referenceValue) * dscale;  // NaN if 2^E overflows
    const double largest = (largestCode * bscale + s.referenceValue) * dscale;
    const double limit = static_cast<double>(std::numeric_limits<Value>::max());
    if (!(std::fabs(smallest) <= limit && std::fabs(largest) <= limit)) {  // false for NaN
        throw std::invalid_argument("Simple packing: the codes cannot be decoded to finite values with this scaling (" +
                                    std::to_string(s.bitsPerValue) + " bits per value, reference value " + std::to_string(s.referenceValue) +
                                    ", binary scale factor " + std::to_string(s.binaryScaleFactor) + ", decimal scale factor " +
                                    std::to_string(s.decimalScaleFactor) + ")");
    }
}

inline Parameters parametersOf(const Scaling& s)
{
    return Parameters(s.bitsPerValue, s.decimalScaleFactor, s.binaryScaleFactor, s.referenceValue, s.bitsPerValue == 0);
}

}  // namespace detail

// Packs count values into packedSize(count, scaling) bytes of output. Floats are scaled in double
// (like doubles): pack of floats gives the codes of pack of the converted doubles
template <typename Value>
void pack(const Value* values, size_t count, const Scaling& scaling, uint8_t* output, size_t capacity)
{
    static_assert(std::is_floating_point<Value>::value, "simple packing packs floating-point values");
    const size_t size = packedSize(count, scaling);
    if (capacity < size) {
        throw std::length_error("Simple packing: the output buffer is too small (" + std::to_string(capacity) + " bytes, " + std::to_string(size) +
                                " bytes are needed)");
    }
    detail::checkDecodable<Value>(scaling);
    detail::SimplePackingBinary<Value> packer(nullptr);
    packer.packTo(detail::parametersOf(scaling), values, count, output);
}

template <typename Value>
std::vector<uint8_t> pack(const std::vector<Value>& values, const Scaling& scaling)
{
    std::vector<uint8_t> output(packedSize(values.size(), scaling));
    pack(values.data(), values.size(), scaling, output.data(), output.size());
    return output;
}

// Unpacks count values from data (at least packedSize(count, scaling) bytes)
template <typename Value>
void unpack(const uint8_t* data, size_t size, const Scaling& scaling, Value* values, size_t count)
{
    static_assert(std::is_floating_point<Value>::value, "simple packing unpacks floating-point values");
    packedSize(count, scaling);  // checks the bits per value
    detail::SimplePackingBinary<Value> packer(nullptr);
    packer.unpackTo(detail::parametersOf(scaling), data, size, count, values);
}

template <typename Value>
std::vector<Value> unpack(const std::vector<uint8_t>& data, size_t count, const Scaling& scaling)
{
    std::vector<Value> values(count);
    unpack(data.data(), data.size(), scaling, values.data(), count);
    return values;
}

}  // namespace sp
