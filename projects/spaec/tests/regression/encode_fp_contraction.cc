// SpAec must map the values to encode with every operation of ((value * 10^D) - R) * 2^-E + 0.5
// rounded separately, like ecCodes built without -march=native. With -march=native, GCC
// contracted value * 10^D - R into a fused multiply-add, which rounds once: values close to a
// rounding boundary were encoded differently (off by one), depending on the build machine.
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "spaec/SpAec.h"

// The mapped value with every operation rounded separately (volatile prevents contraction)
static uint32_t expectedMapped(double value, int decimalScale, int binaryScale, double reference)
{
    volatile double product = value * grib_power<double>(decimalScale, 10);
    volatile double difference = product - reference;
    volatile double scaled = difference * grib_power<double>(-binaryScale, 2);
    volatile double rounded = scaled + 0.5;
    return static_cast<uint32_t>(rounded);
}

int main()
{
    // Values found to be mapped differently with a fused multiply-add, and their correct mapped values
    struct Case
    {
        double value;
        int decimalScale;
        int binaryScale;
        double reference;
        uint32_t mapped;
    };
    const Case cases[] = { { 13136.093945312499, 1, -8, 99646, 8119025 },
                           { 1263.81806640625, 2, -8, 69140, 14653903 },
                           { 6890.2935546874996, 1, -8, 65279, 927728 },
                           { 1025.2067187499999, 2, -5, 54860, 1525142 },
                           { 1211.4638085937499, 2, -8, 87227, 8683362 } };

    std::vector<double> values;
    std::vector<uint32_t> expected;
    std::vector<int> decimalScales, binaryScales;
    std::vector<double> references;
    for (const auto& c : cases) {
        values.push_back(c.value);
        expected.push_back(c.mapped);
        decimalScales.push_back(c.decimalScale);
        binaryScales.push_back(c.binaryScale);
        references.push_back(c.reference);
    }
    // Values half-way between two mapped values (before rounding the division)
    std::mt19937_64 rng(42);
    for (int i = 0; i < 5000; ++i) {
        const int decimalScale = 1 + static_cast<int>(rng() % 3);
        const int binaryScale = -static_cast<int>(rng() % 12);
        const double reference = static_cast<double>(rng() % 100000);
        const double k = static_cast<double>(rng() % (1u << 23));
        const double value = (reference + std::ldexp(k + 0.5, binaryScale)) / grib_power<double>(decimalScale, 10);
        values.push_back(value);
        expected.push_back(expectedMapped(value, decimalScale, binaryScale, reference));
        decimalScales.push_back(decimalScale);
        binaryScales.push_back(binaryScale);
        references.push_back(reference);
    }

    // Encode each value (repeated to a block) with its parameters; decode the mapped value
    int failures = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        constexpr uint8_t bpv = 24, blockSize = 8;
        std::vector<double> data(blockSize, values[i]);
        encoder::SpAec<double> spaec(bpv, blockSize, 1, CodeOptionSet::Basic, references[i], binaryScales[i], decimalScales[i]);
        std::vector<uint8_t> encoded(spaec.maxEncodedSize(data.size()));
        const size_t length = spaec.encode(Span<double>(data), Span<uint8_t>(encoded));

        DecoderSeq<uint32_t, uint32_t> decoder(bpv, blockSize, 1, CodeOptionSet::Basic);
        std::vector<uint32_t> mapped(data.size());
        decoder.decode(Span<uint8_t>(encoded.data(), length), Span<uint32_t>(mapped));
        if (mapped[0] != expected[i]) {
            if (failures < 20) {
                std::cerr << "value " << values[i] << " (D=" << decimalScales[i] << ", E=" << binaryScales[i] << ", R=" << references[i]
                          << "): mapped to " << mapped[0] << ", expected " << expected[i] << '\n';
            }
            ++failures;
        }
    }

    if (failures != 0) {
        std::cerr << failures << " of " << values.size() << " values mapped with a different rounding\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
