// Computing packing parameters without an optimiser (decode-only instance) must throw
// std::logic_error instead of dereferencing a null pointer.

#include <iostream>
#include <stdexcept>
#include <vector>

#include "simple_packing/simple/Binary.h"

using namespace sp;

int main()
{
    SimplePackingBinary<double> sp(nullptr);
    std::vector<double> values = { 1.0, 2.0, 3.0 };
    try {
        sp.pack(values);
        std::cerr << "FAIL: pack without optimiser was accepted\n";
        return 1;
    }
    catch (const std::logic_error& e) {
        std::cout << "OK: " << e.what() << '\n';
    }
    return 0;
}
