#pragma once

#include <Parameters.h>

class Optimizer {
    public:
        virtual ~Optimizer() = default;
        virtual Parameters getParams(double min, double max) = 0;
};


