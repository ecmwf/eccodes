/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <cstdint>

namespace sp::detail
{

// The packing parameters of GRIB simple packing: bits per value, binary and
// decimal scale factors, reference value, and whether the field is constant.

struct Parameters
{
public:
    Parameters() : bitsPerValue_(0), decimalScaleFactor_(0), binaryScaleFactor_(0), referenceValue_(0.0), isConstantField_(false) {}
    Parameters(int64_t bitsPerValue, int64_t decimalScaleFactor, int64_t binaryScaleFactor, double referenceValue, bool isConstantField) :
        bitsPerValue_(bitsPerValue), decimalScaleFactor_(decimalScaleFactor), binaryScaleFactor_(binaryScaleFactor), referenceValue_(referenceValue), isConstantField_(isConstantField) {}

    int64_t bitsPerValue() const { return bitsPerValue_; }
    void bitsPerValue(int64_t bitsPerValue) { bitsPerValue_ = bitsPerValue; }
    int64_t decimalScaleFactor() const { return decimalScaleFactor_; }
    void decimalScaleFactor(int64_t decimalScaleFactor) { decimalScaleFactor_ = decimalScaleFactor; }
    int64_t binaryScaleFactor() const { return binaryScaleFactor_; }
    void binaryScaleFactor(int64_t binaryScaleFactor) { binaryScaleFactor_ = binaryScaleFactor; }
    double referenceValue() const { return referenceValue_; }
    void referenceValue(double referenceValue) { referenceValue_ = referenceValue; }
    bool isConstantField() const { return isConstantField_; }
    void isConstantField(bool isConstantField) { isConstantField_ = isConstantField; }

private:
    int64_t bitsPerValue_;
    int64_t decimalScaleFactor_;
    int64_t binaryScaleFactor_;
    double referenceValue_;
    bool isConstantField_;
};

}  // namespace sp::detail
