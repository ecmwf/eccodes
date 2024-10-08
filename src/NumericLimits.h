/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include <array>
#include <cstdint>
#include <climits> // CHAR_BIT

// NumericLimits is a class template that provides the minimum and maximum values for a given number of bits.
// The minimum and maximum values are calculated for both signed and unsigned integral types.

// Example:
// For a 16-bit signed integer, the minimum and maximum values are:
// nbits | min | max
// ----- | --- | ---
// 1     | -1  | 0
// 2     | -2  | 1
// 3     | -4  | 3
// 4     | -8  | 7
// ...
// 15    | -16384 | 16383
// 16    | -32768 | 32767

template <typename ValueType, bool = std::is_signed<ValueType>::value, bool = std::is_integral<ValueType>::value>
struct NumericLimits
{
private:
    static constexpr uint8_t MaxN = CHAR_BIT * sizeof(ValueType);

    static constexpr std::array<ValueType, MaxN> max_ = []() {
        std::array<ValueType, MaxN> max{};
        using UnsignedValueType          = std::make_unsigned_t<ValueType>;
        constexpr UnsignedValueType ones = ~UnsignedValueType{0};
        max[0]                           = 0;
        for (uint8_t i = 1; i < MaxN; ++i) {
            max[i] = static_cast<ValueType>(ones >> (MaxN - i));
        }
        return max;
    }();

    static constexpr std::array<ValueType, MaxN> min_ = []() {
        std::array<ValueType, MaxN> min{};
        for (uint8_t i = 0; i < MaxN; ++i) {
            min[i] = ~max_[i];
        }
        return min;
    }();

public:
    static constexpr ValueType min(uint8_t nbits)
    {
        return min_[nbits - 1];
    }

    static constexpr ValueType max(uint8_t nbits)
    {
        return max_[nbits - 1];
    }
};


// Example:
// For a 16-bit unsigned integer, the minimum and maximum values are:
// nbits | min | max
// ----- | --- | ---
// 1     | 0   | 1
// 2     | 0   | 3
// 3     | 0   | 7
// 4     | 0   | 15
// ...
// 15    | 0   | 32767
// 16    | 0   | 65535

template <typename ValueType>
struct NumericLimits<ValueType, false, true>
{
    static_assert(std::is_integral<ValueType>::value, "ValueType must be an integral type");

private:
    static constexpr uint8_t MaxN = CHAR_BIT * sizeof(ValueType);

    static constexpr std::array<ValueType, MaxN> max_ = []() {
        std::array<ValueType, MaxN> max{};
        constexpr ValueType ones = ~(static_cast<ValueType>(0));
        max[0]                   = 1;
        for (uint8_t i = 1; i < MaxN; ++i) {
            max[i] = ones >> (MaxN - i - 1);
        }
        return max;
    }();

public:
    static constexpr ValueType min(uint8_t nbits)
    {
        return 0;
    }

    static constexpr ValueType max(uint8_t nbits)
    {
        return max_[nbits - 1];
    }
};
