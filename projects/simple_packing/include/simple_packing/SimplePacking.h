/*
 * (C) Copyright 2023- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

#include <cstddef>
#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <memory>
#include <memory>
#include <variant>

#include "simple_packing/Parameters.h"
#include "simple_packing/Optimizer.h"
#include "simple_packing/Utils.h"
#include "simple_packing/codec/Binary.h"
#include "simple_packing/codec/Array.h"


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

        if (check_data_values_range(min, max) != SP_SUCCESS) {
            throw std::runtime_error("Values out of range (minimum " + std::to_string(min) + ", maximum " + std::to_string(max) + "): values must be finite");
        }

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
            params = optimizer_->getParams(min, max);
        }
        return std::make_tuple(params, min, max);
    }

private:
    std::unique_ptr<Optimizer> optimizer_;
};
