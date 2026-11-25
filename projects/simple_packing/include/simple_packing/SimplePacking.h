/*
 * SPDX-FileCopyrightText: 2023 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "simple_packing/Parameters.h"
#include "simple_packing/Optimizer.h"
#include "simple_packing/Utils.h"

namespace sp
{


template <typename ValueType>
class SimplePacking
{
public:
    SimplePacking(std::unique_ptr<Optimizer> optimizer) : optimizer_(std::move(optimizer)) {}
    ~SimplePacking() = default;

    std::tuple<Parameters, ValueType, ValueType> getParameters(const std::vector<ValueType>& values)
    {
        return getParameters(values.data(), values.size());
    }

    std::tuple<Parameters, ValueType, ValueType> getParameters(const ValueType* values, size_t nValues)
    {
        auto [min, max] = sp_minmax(values, nValues);
        return getParameters(min, max);
    }

    // Overload for callers that already know the range of the field, so
    // that it is not scanned twice.
    std::tuple<Parameters, ValueType, ValueType> getParameters(ValueType min, ValueType max)
    {
        Parameters params;

        check_data_values_range(min, max);

        if (max == min) {
            auto ref = nearest_smaller_value(min, optimizer_ ? optimizer_->referenceFloatType() : FloatType::IEEE32);
            if (ref.has_value()) {
                params.referenceValue(ref.value());
            }
            else {
                throw std::runtime_error("unable to find nearest_smaller_value");
            }
            params.bitsPerValue(0);
            params.binaryScaleFactor(0);
            params.decimalScaleFactor(0);
            params.isConstantField(true);
            return std::make_tuple(params, min, max);
        }
        else {
            if (!optimizer_) {
                throw std::logic_error("Simple packing: no optimiser to compute the packing parameters (created for decoding only)");
            }
            params = optimizer_->getParams(min, max);
        }
        return std::make_tuple(params, min, max);
    }

private:
    std::unique_ptr<Optimizer> optimizer_;
};

}  // namespace sp
