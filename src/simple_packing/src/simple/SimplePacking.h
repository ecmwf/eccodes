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
#include <algorithm>
#include <memory>
#include <memory>
#include <variant>

#include "LibEccodes.h"
#include <Parameters.h>
#include <Optimizer.h>
#include <Utils.h>
#include <codec/Binary.h>
#include <codec/Array.h>


enum class FloatType {
    IEEE32,
    IBM32,
    UNKNOWN
};



template <typename ValueType>
class SimplePacking {
    public:
        SimplePacking(std::unique_ptr<Optimizer> optimizer) : optimizer_(std::move(optimizer)) {}
        ~SimplePacking() = default;

        std::tuple<Parameters, ValueType, ValueType> getParameters(const std::vector<ValueType>& values) {
            Parameters params;

            auto [min_ptr, max_ptr] = std::minmax_element(values.begin(), values.end());
            auto min = *min_ptr;
            auto max = *max_ptr;
            // auto [min, max] = minmax_fast<ValueType>(values.data(), values.size());

            int err;
            if (grib_check_data_values_range(min, max) != LIB_ECCODES_SUCCESS) {
                throw std::runtime_error("Invalid data values range");
            }

            if (max == min) {
                auto ref = nearest_smaller_value(min);
                if (ref.has_value()){
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
