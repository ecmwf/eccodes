#include <iostream>
#include <cmath>

// #include <LibEccodes.h>

template <typename T>
T grib_power(long s, long n)
{
    T divisor = 1.0;
    if (s == 0)
        return 1.0;
    if (s == 1)
        return n;
    while (s < 0) {
        divisor /= n;
        s++;
    }
    while (s > 0) {
        divisor *= n;
        s--;
    }
    return divisor;
}

int main(int argc, char** argv) {

    for (int bpv = 0; bpv <= 64; bpv++) {
        std::cout << std::fixed << grib_power<double>(bpv, 2) << std::endl;
        std::cout << pow(2, bpv) << std::endl;
    }

}
