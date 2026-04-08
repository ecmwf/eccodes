#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <random>

#include <BitPacking.h>
#include <Utils.h>



template <uint8_t BPV, typename ValueType = typename BitPacking<BPV>::ValueType>
void run_test()
{
    size_t nVals = 27;
    BitPacking<BPV> bv;
    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<typename BitPacking<BPV>::ValueType> dist(0, (1ULL << BPV) - 1);

    std::vector<ValueType> values(nVals);
    for (size_t nval = 0; nval < nVals; ++nval) {
        values[nval] = dist(rng);
    }

    ValueType val = dist(rng);
    size_t pos = dist(rng) % nVals;

    std::vector<ValueType> expected_values = values;
    expected_values[pos] = val;
    std::cout << "Inserting value " << static_cast<uint64_t>(val) << " at position " << pos << std::endl;

    std::vector<uint8_t> data(((BPV * nVals) + 7) / 8, 0);

    for (size_t nval = 0, nbit = 0; nval < nVals; nbit += BPV, ++nval) {
        bv.encode(&data[nbit / 8], nbit % 8, values[nval]);
    }

    bv.encode_element(&data[(pos * BPV) / 8], (pos * BPV) % 8, val);

    auto decoded_val = bv.decode_element(&data[(pos * BPV) / 8], (pos * BPV) % 8);

    std::vector<ValueType> decoded_values(values.size(), 0);
    for (size_t nval = 0, nbit = 0; nval < nVals; nbit += BPV, ++nval) {
        decoded_values[nval] = bv.decode(&data[nbit / 8], nbit % 8);
    }
    //print values and new values and expected values
    std::cout << "Decoded inserted value: " << static_cast<uint64_t>(decoded_val) << std::endl;

    for (size_t i = 0; i < nVals; ++i) {
        std::cout 
            << "Value[" << i << "] = " << static_cast<uint64_t>(values[i]) 
            << ", new value[" << i << "] = " << static_cast<uint64_t>(decoded_values[i]) 
            << ", expected = " << static_cast<uint64_t>(expected_values[i]) << std::endl;
    }
    if (std::equal(std::begin(expected_values), std::end(expected_values), std::begin(decoded_values))) {
        std::cout << "Test passed for BPV = " << static_cast<int>(BPV) << std::endl;
    } else {
        throw std::runtime_error("Test failed: values do not match after insertion");
    }
}



int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [BPV]" << std::endl;
        return 1;
    }

    uint8_t bpv = static_cast<uint8_t>(std::stoi(argv[1]));

    run_test<10>();
    return 0;
}
