#pragma once

#include <cstddef>
#include <Parameters.h>
#include <Optimizer.h>


class BinDecOptimizer : public Optimizer {
    public:
        BinDecOptimizer(const int compat_gribex, const int compat_32bit, const int knbit)
            : compat_gribex_(compat_gribex), compat_32bit_(compat_32bit), knbit_(knbit) {}
        ~BinDecOptimizer() = default;
        Parameters getParams(double min, double max);

    private:
        int compat_gribex_ = 0;
        int compat_32bit_ = 0;
        int knbit_ = 0;
};

void factec(int* krep, const double pa, const int knbit, const long kdec, const int range, long* ke, int* knutil);
// constexpr double epsilon();
int vrange();
