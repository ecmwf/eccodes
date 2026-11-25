#include <cassert>
#include <bitset>
#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <unordered_map>

#include "simple_packing/BitPacking.h"

using namespace sp;


template <uint8_t BPV, typename ValueType = typename BitPacking<BPV>::ValueType>
void test_bpv(const std::vector<ValueType>& expected_values, const std::vector<uint8_t>& expected_data)
{
    size_t nVals = expected_values.size();
    BitPacking<BPV> bv;

    std::vector<uint8_t> data(expected_data.size(), 0);
    for (size_t nval = 0, nbit = 0; nval < nVals; nbit += BPV, ++nval) {
        bv.encode(&data[nbit / 8], nbit % 8, expected_values[nval]);
    }
    if (!std::equal(std::begin(expected_data), std::end(expected_data), std::begin(data))) {
        std::cerr << "Expected data: ";
        for (auto byte : expected_data) {
            std::cerr << std::bitset<8>(byte) << " ";
        }
        std::cerr << std::endl;
        std::cerr << "Encoded data:  ";
        for (auto byte : data) {
            std::cerr << std::bitset<8>(byte) << " ";
        }
        std::cerr << std::endl;
        throw std::runtime_error("Encoded data does not match expected data");
    }


    std::vector<ValueType> values(expected_values.size(), 0);
    for (size_t nval = 0, nbit = 0; nval < nVals; nbit += BPV, ++nval) {
        values[nval] = bv.decode(&data[nbit / 8], nbit % 8);
    }
    if (!std::equal(std::begin(expected_values), std::end(expected_values), std::begin(values))) {
        std::cerr << "Expected values: ";
        for (auto val : expected_values) {
            std::cerr << static_cast<uint64_t>(val) << " ";
        }
        std::cerr << std::endl;
        std::cerr << "Decoded values:  ";
        for (auto val : values) {
            std::cerr << static_cast<uint64_t>(val) << " ";
        }
        std::cerr << std::endl;
        throw std::runtime_error("Decoded values do not match expected values");
    }
}


// Calls f(std::integral_constant<std::size_t, I>{}) for every I in Is (a template lambda in C++20)
template <typename F, std::size_t... Is>
static void dispatchAll(const F& f, std::index_sequence<Is...>)
{
    (f(std::integral_constant<std::size_t, Is>{}), ...);
}

void run_test(uint8_t bpv)
{
    std::unordered_map<uint8_t, std::vector<std::pair<std::vector<uint64_t>, std::vector<uint8_t>>>> test_data = {
#include "bit_packing_data.cc"
    };
    const auto tests = test_data[bpv];

    // Dispatch to the codec with the given bits per value
    const auto dispatch = [&](auto index) {
        constexpr uint8_t BPV = static_cast<uint8_t>(decltype(index)::value + 1);
        if (bpv == BPV) {
            for (const auto& [expected_values, expected_data] : tests) {
                test_bpv<BPV>(expected_values, expected_data);
            }
        }
    };
    dispatchAll(dispatch, std::make_index_sequence<64>{});

    if (bpv < 1 || bpv > 64) {
        throw std::runtime_error("Unsupported BPV in test data");
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [BPV]" << std::endl;
        return 1;
    }

    uint8_t bpv = static_cast<uint8_t>(std::stoi(argv[1]));
    run_test(bpv);
    return 0;
}
