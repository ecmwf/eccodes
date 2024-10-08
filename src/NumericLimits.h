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

template <typename ValueType>
struct NumericLimits
{
    static_assert(std::is_integral<ValueType>::value, "ValueType must be an integral type");

private:
    using S                   = std::make_signed_t<ValueType>;
    using U                   = std::make_unsigned_t<ValueType>;
    static constexpr int MaxN = 8 * sizeof(ValueType);

    static constexpr std::array<ValueType, MaxN> max_ = []() {
        if constexpr (std::is_signed<ValueType>::value) {
            std::array<S, MaxN> max{};
            for (int i = 1; i < MaxN; ++i) {
                auto ones = ~(static_cast<U>(0));
                max[i]    = static_cast<S>(ones >> (MaxN - i));
            }
            return max;
        }
        else {
            std::array<U, MaxN> max{};
            for (int i = 1; i < MaxN; ++i) {
                auto ones = ~(static_cast<U>(0));
                max[i]    = ones >> (MaxN - i - 1);
            }
            return max;
        }
    }();

    static constexpr std::array<ValueType, MaxN> min_ = []() {
        if constexpr (std::is_signed<ValueType>::value) {
            std::array<S, MaxN> min{};
            for (int i = 1; i < MaxN; ++i) {
                min[i] = ~max_[i];
            }
            return min;
        }
        else {
            return std::array<U, MaxN>{};
        }
    }();

public:
    static constexpr ValueType min(int nbits)
    {
        if constexpr (std::is_signed<ValueType>::value)
            return min_[nbits - 1];
        else
            return 0;
    }

    static constexpr ValueType max(int nbits)
    {
        return max_[nbits - 1];
    }
};
