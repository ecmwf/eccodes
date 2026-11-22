// Printing the statistics of an empty field must not crash (it indexed and divided by
// the number of blocks, which is 0).

#include <cstdint>
#include <iostream>
#include <vector>

#include "ccsds/SpAec.h"

int main()
{
    encoder::SpAec<double> spaec{ 16, 32, 64, CodeOptionSet::Basic, 0.0 };
    std::vector<uint8_t> stream;
    std::vector<double> values;
    spaec.decode(Span<uint8_t>(stream), Span<double>(values));

    auto stats = spaec.getStats();
    stats.print();
    stats.printMetrics();
    std::cout << "OK\n";
    return 0;
}
