#pragma once

#include "simple_packing/Parameters.h"
#include "simple_packing/Utils.h"

class Optimizer
{
public:
    virtual ~Optimizer() = default;
    virtual Parameters getParams(double min, double max) = 0;

    // Format in which the reference value is stored (it is rounded down to a value of this format)
    void referenceFloatType(FloatType type) { referenceFloatType_ = type; }
    FloatType referenceFloatType() const { return referenceFloatType_; }

protected:
    FloatType referenceFloatType_ = FloatType::IEEE32;
};
