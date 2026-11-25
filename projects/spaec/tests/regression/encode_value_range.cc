// Values that cannot be packed with the given bits per value, reference value and scale factors
// (NaN, infinity, below the reference value, too large) must be rejected: the conversion used
// to be undefined behaviour and produced wrong data silently.

#include <cstddef>
#include <exception>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/SpAec.h"

using namespace spaec;

int main()
{
    try {
        int failures = 0;

        // 8 bits, reference 0, no scaling: values must be in [0, 255]
        encoder::SpAec<double> spaec{ 8, 32, 64, CodeOptionSet::Basic, 0.0 };
        std::vector<uint8_t> target(encoder::SpAec<double>::maxEncodedSize(100, 8, 32));

        for (double bad : { std::nan(""), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -1.0, 256.0, 1e300 }) {
            std::vector<double> data(100, 1.0);
            data[42] = bad;
            try {
                spaec.encode(Span<double>(data), Span<uint8_t>(target));
                std::cerr << "FAIL: value " << bad << " was accepted\n";
                failures++;
            }
            catch (const std::invalid_argument& e) {
                std::cout << "OK: " << e.what() << '\n';
            }
        }

        // The limits are accepted
        std::vector<double> data(100, 0.0);
        data[1] = 255.0;
        data[2] = 255.4;  // rounds to 255
        try {
            const size_t n = spaec.encode(Span<double>(data), Span<uint8_t>(target));
            std::vector<double> decoded(100);
            spaec.decode(Span<uint8_t>(target.data(), n), Span<double>(decoded));
            if (decoded[1] != 255.0 || decoded[2] != 255.0) {
                std::cerr << "FAIL: limits decoded wrongly\n";
                failures++;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "FAIL: valid values were rejected: " << e.what() << '\n';
            failures++;
        }

        return failures == 0 ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
