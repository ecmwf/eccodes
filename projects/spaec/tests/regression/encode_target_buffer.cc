// The encoder must reject an output buffer that is too small (it used to write past its end)
// and must not depend on the previous content of the output buffer.

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <exception>
#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/SpAec.h"

int main()
{
    try {
        int failures = 0;

        const size_t n = 1024;
        std::vector<double> data(n);
        for (size_t i = 0; i < n; ++i) {
            data[i] = static_cast<double>((i * 7919) % 65536);  // hardly compressible
        }
        encoder::SpAec<double> spaec{ 16, 32, 64, CodeOptionSet::Basic, 0.0 };

        for (size_t size : { 0, 1, 8, 100, 2000 }) {
            std::vector<uint8_t> target(size);
            try {
                spaec.encode(Span<double>(data), Span<uint8_t>(target));
                std::cerr << "FAIL: output buffer of " << size << " bytes was accepted\n";
                failures++;
            }
            catch (const std::invalid_argument& e) {
                std::cout << "OK: " << e.what() << '\n';
            }
        }

        // An output buffer with old content
        std::vector<uint8_t> target(n * 8, 0xff);
        const size_t encodedSize = spaec.encode(Span<double>(data), Span<uint8_t>(target));
        std::vector<double> decoded(n);
        spaec.decode(Span<uint8_t>(target.data(), encodedSize), Span<double>(decoded));
        if (decoded != data) {
            std::cerr << "FAIL: encoding into a used buffer gives wrong data\n";
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
