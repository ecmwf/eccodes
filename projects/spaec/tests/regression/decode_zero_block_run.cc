// A zero-block run longer than the rest of the segment must be reported as an error
// (it used to overflow the decoder's segment buffer).

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <exception>
#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/SpAec.h"

using namespace spaec;

int main()
{
    try {
        int failures = 0;

        // bpv=8, block size 32, RSI 1: the stream holds a zero block (ID 0000), the reference sample
        // and a codeword of 59 zero bits, i.e. a run of ~60 zero blocks in a segment of 1 block.
        {
            std::vector<uint8_t> stream = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
            std::vector<double> values(32);
            encoder::SpAec<double> spaec{ 8, 32, 1, CodeOptionSet::Basic, 0.0 };
            try {
                spaec.decode(Span<uint8_t>(stream), Span<double>(values));
                std::cerr << "FAIL: zero-block run beyond the segment was not reported\n";
                failures++;
            }
            catch (const std::runtime_error& e) {
                std::cout << "OK: " << e.what() << '\n';
            }
        }

        // A valid all-zero field still decodes
        {
            std::vector<double> data(1000, 0.0);
            encoder::SpAec<double> spaec{ 8, 32, 64, CodeOptionSet::Basic, 0.0 };
            std::vector<uint8_t> encoded(data.size() * 2 + 1024);
            const size_t n = spaec.encode(Span<double>(data), Span<uint8_t>(encoded));
            std::vector<double> decoded(data.size(), 1.0);
            spaec.decode(Span<uint8_t>(encoded.data(), n), Span<double>(decoded));
            for (double v : decoded) {
                if (v != 0.0) {
                    std::cerr << "FAIL: all-zero field decoded wrongly\n";
                    failures++;
                    break;
                }
            }
        }

        return failures == 0 ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
