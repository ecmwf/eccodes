#pragma once

#include "simple_packing/Parameters.h"

class Optimizer
{
public:
    virtual ~Optimizer() = default;
    virtual Parameters getParams(double min, double max) = 0;
};
