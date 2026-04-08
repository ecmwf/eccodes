#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <BitPacking.h>
#include <Utils.h>



template <uint8_t BPV, typename ValueType = typename BitPacking<BPV>::ValueType>
void test_bpv(const std::vector<ValueType>& expected_values, const std::vector<uint8_t>& expected_data) {

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


void run_test(uint8_t bpv) {
    std::unordered_map<uint8_t, std::vector<std::pair<std::vector<uint64_t>, std::vector<uint8_t>>>> test_data = {
#include "bit_packing_data.cc"
    };
    const auto tests = test_data[bpv];

    for (const auto& [expected_values, expected_data] : tests) {
        switch (bpv) {
            case 1: test_bpv<1>(expected_values, expected_data); break;
            case 2: test_bpv<2>(expected_values, expected_data); break;
            case 3: test_bpv<3>(expected_values, expected_data); break;
            case 4: test_bpv<4>(expected_values, expected_data); break;
            case 5: test_bpv<5>(expected_values, expected_data); break;
            case 6: test_bpv<6>(expected_values, expected_data); break;
            case 7: test_bpv<7>(expected_values, expected_data); break;
            case 8: test_bpv<8>(expected_values, expected_data); break;
            case 9: test_bpv<9>(expected_values, expected_data); break;
            case 10: test_bpv<10>(expected_values, expected_data); break;
            case 11: test_bpv<11>(expected_values, expected_data); break;
            case 12: test_bpv<12>(expected_values, expected_data); break;
            case 13: test_bpv<13>(expected_values, expected_data); break;
            case 14: test_bpv<14>(expected_values, expected_data); break;
            case 15: test_bpv<15>(expected_values, expected_data); break;
            case 16: test_bpv<16>(expected_values, expected_data); break;
            case 17: test_bpv<17>(expected_values, expected_data); break;
            case 18: test_bpv<18>(expected_values, expected_data); break;
            case 19: test_bpv<19>(expected_values, expected_data); break;
            case 20: test_bpv<20>(expected_values, expected_data); break;
            case 21: test_bpv<21>(expected_values, expected_data); break;
            case 22: test_bpv<22>(expected_values, expected_data); break;
            case 23: test_bpv<23>(expected_values, expected_data); break;
            case 24: test_bpv<24>(expected_values, expected_data); break;
            case 25: test_bpv<25>(expected_values, expected_data); break;
            case 26: test_bpv<26>(expected_values, expected_data); break;
            case 27: test_bpv<27>(expected_values, expected_data); break;
            case 28: test_bpv<28>(expected_values, expected_data); break;
            case 29: test_bpv<29>(expected_values, expected_data); break;
            case 30: test_bpv<30>(expected_values, expected_data); break;
            case 31: test_bpv<31>(expected_values, expected_data); break;
            case 32: test_bpv<32>(expected_values, expected_data); break;
            case 33: test_bpv<33>(expected_values, expected_data); break;
            case 34: test_bpv<34>(expected_values, expected_data); break;
            case 35: test_bpv<35>(expected_values, expected_data); break;
            case 36: test_bpv<36>(expected_values, expected_data); break;
            case 37: test_bpv<37>(expected_values, expected_data); break;
            case 38: test_bpv<38>(expected_values, expected_data); break;
            case 39: test_bpv<39>(expected_values, expected_data); break;
            case 40: test_bpv<40>(expected_values, expected_data); break;
            case 41: test_bpv<41>(expected_values, expected_data); break;
            case 42: test_bpv<42>(expected_values, expected_data); break;
            case 43: test_bpv<43>(expected_values, expected_data); break;
            case 44: test_bpv<44>(expected_values, expected_data); break;
            case 45: test_bpv<45>(expected_values, expected_data); break;
            case 46: test_bpv<46>(expected_values, expected_data); break;
            case 47: test_bpv<47>(expected_values, expected_data); break;
            case 48: test_bpv<48>(expected_values, expected_data); break;
            case 49: test_bpv<49>(expected_values, expected_data); break;
            case 50: test_bpv<50>(expected_values, expected_data); break;
            case 51: test_bpv<51>(expected_values, expected_data); break;
            case 52: test_bpv<52>(expected_values, expected_data); break;
            case 53: test_bpv<53>(expected_values, expected_data); break;
            case 54: test_bpv<54>(expected_values, expected_data); break;
            case 55: test_bpv<55>(expected_values, expected_data); break;
            case 56: test_bpv<56>(expected_values, expected_data); break;
            case 57: test_bpv<57>(expected_values, expected_data); break;
            case 58: test_bpv<58>(expected_values, expected_data); break;
            case 59: test_bpv<59>(expected_values, expected_data); break;
            case 60: test_bpv<60>(expected_values, expected_data); break;
            case 61: test_bpv<61>(expected_values, expected_data); break;
            case 62: test_bpv<62>(expected_values, expected_data); break;
            case 63: test_bpv<63>(expected_values, expected_data); break;
            case 64: test_bpv<64>(expected_values, expected_data); break;

            default:
                     throw std::runtime_error("Unsupported BPV in test data");
        }
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
