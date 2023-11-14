#pragma once

// Utils to help convert the accessor C code to C++

namespace eccodes::accessor
{

/* Return n to the power of s */
template <typename T>
constexpr T codesPower(long s, long n)
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

}