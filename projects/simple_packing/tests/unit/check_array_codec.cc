#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>


#include "simple_packing/optimizer/DecOptimizer.h"
#include "simple_packing/optimizer/BPVOptimizer.h"
#include "simple_packing/simple/Array.h"

using namespace sp;


template <typename ValueType>
void encode_decode(size_t nVals, size_t bpv)
{
    using SP = SimplePackingArray<ValueType>;

    std::cerr << "Encoding and decoding " << nVals << " values with bpv = " << bpv << std::endl;


    int last = 127; /* 'last' should be a parameter coming from a definitions file */
    std::unique_ptr<Optimizer> optimizer = std::make_unique<DecOptimizer>(bpv, last);

    SP sp{ std::move(optimizer) };
    typename SP::Values original;

    for (unsigned int i = 0; i < nVals; ++i) {
        original.push_back((i + 3) % 5);
    }

    auto [params, encoded] = sp.pack(original);

    typename SP::Values decoded = sp.unpack(params, encoded, original.size());

    assert(decoded.size() == original.size());
    auto failed = false;
    for (unsigned int i = 0; i < original.size(); ++i) {
        if (original[i] != decoded[i]) {
            failed = true;
            break;
        }
    }

    if (failed) {
        auto size = std::min(original.size(), size_t(30));
        for (size_t j = 0; j < size; ++j) {
            std::cerr << "original[" << j << "] = " << original[j] << " decoded[" << j << "] = " << decoded[j] << std::endl;
        }
        throw std::runtime_error("Decoded data does not match original data");
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <type> <nVals> <bpv>" << std::endl;
        return 1;
    }


    std::string type = argv[1];
    size_t nVals = std::stoul(argv[2]);
    size_t bpv = std::stoul(argv[3]);

    if (type == "float")
        encode_decode<float>(nVals, bpv);
    else if (type == "double")
        encode_decode<double>(nVals, bpv);

    return 0;
}
