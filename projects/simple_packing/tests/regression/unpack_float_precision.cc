// Unpacking into float must compute like the legacy ecCodes code: the scaling is done in double
// (with the powers of 2 and 10 computed in float) and the result is rounded to float once.
// Computing in float loses the low bits of values with more than 24 bits and values near zero
// that result from a large negative reference value came out completely wrong (0 instead of 1.75).
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "simple_packing/Utils.h"
#include "simple_packing/simple/Binary.h"

// Packs the values MSB first with bpv bits each, like GRIB simple packing
static std::vector<unsigned char> packBits(const std::vector<uint64_t>& values, int bpv)
{
    std::vector<unsigned char> out((values.size() * bpv + 7) / 8 + 8, 0);
    size_t bit = 0;
    for (uint64_t v : values) {
        for (int b = bpv - 1; b >= 0; --b, ++bit) {
            if ((v >> b) & 1u) {
                out[bit / 8] |= static_cast<unsigned char>(0x80u >> (bit % 8));
            }
        }
    }
    return out;
}

int main()
{
    struct Case
    {
        int bpv;
        double reference;
        int binaryScale;
        int decimalScale;
    };
    // From ecCodes messages whose float values were wrong, plus wider ranges
    const Case cases[] = { { 30, -152.5, -22, 0 }, { 32, -9.9997, -27, 0 }, { 26, -4646.09, -13, 0 }, { 30, -10000.0, -15, 0 },
                           { 32, -3.0e6, -8, 3 }, { 25, 12.5, -20, 1 }, { 16, -100.0, -4, 2 }, { 8, 1.0, 0, -2 } };
    const unsigned seed = std::random_device{}();
    std::mt19937_64 rng(seed);
    size_t failures = 0;
    for (const Case& c : cases) {
        const size_t n = 1003;
        std::vector<uint64_t> coded(n);
        std::uniform_int_distribution<uint64_t> any(0, (uint64_t{ 1 } << c.bpv) - 1);
        for (size_t i = 0; i < n; ++i) {
            coded[i] = i % 5 == 0 ? 0 : any(rng);
        }
        const std::vector<unsigned char> buffer = packBits(coded, c.bpv);
        const double s = sp_power<float>(c.binaryScale, 2);
        const double d = sp_power<float>(-c.decimalScale, 10);

        std::vector<float> values(n);
        SimplePackingBinary<float> sp(nullptr);
        sp.unpackTo(Parameters(c.bpv, c.decimalScale, c.binaryScale, c.reference, false), buffer.data(), buffer.size(), n, values.data());
        for (size_t i = 0; i < n; ++i) {
            const double exact = (static_cast<double>(coded[i]) * s + c.reference) * d;
            const float expected = static_cast<float>(exact);
            if (values[i] != expected && ++failures <= 5) {
                std::cerr << std::setprecision(9) << "FAIL bpv=" << c.bpv << " index " << i << ": unpacked " << values[i]
                          << ", expected " << expected << " (exact " << exact << ")\n";
            }
        }
    }
    if (failures != 0) {
        std::cerr << failures << " wrong float values (seed " << seed << ")\n";
        return 1;
    }
    return 0;
}
