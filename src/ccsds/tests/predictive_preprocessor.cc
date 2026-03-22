#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>

#include "Preprocessor.h"


// void checkSignedPreprocessor() {
//     std::vector<int16_t> data = {100, 105, 110, 120, 125};
//     std::vector<int16_t> restored = data; // For potential future use
//     std::vector<int16_t> temp(data.size());
//
//     PredictivePreprocessing<16, int16_t, UnitDelayPredictor, Mapper<16, uint16_t>> preprocessorEncode;
//     preprocessorEncode.encode(data, temp);
//     PredictivePreprocessing<16, int16_t, UnitDelayPredictor, Mapper<16, uint16_t>> preprocessorDecode;
//     preprocessorDecode.decode(temp, restored);
//
//     for (size_t i = 0; i < data.size(); ++i) {
//         std::cout
//             << "Original: " << data[i]
//             << ", Encoded: " << temp[i]
//             << ", Restored: " << restored[i]
//             << std::endl;
//     }
//
//     for (size_t i = 0; i < data.size(); ++i) {
//         if (data[i] != restored[i]) {
//             std::cerr << "Error at index " << i << ": original " << data[i] << ", restored " << restored[i] << std::endl;
//             assert(false);
//         }
//     }
//
// }
//
// void checkUnsignedPreprocessor() {
//     std::vector<uint16_t> data = {100, 105, 110, 120, 125};
//     std::vector<uint16_t> restored = data; // For potential future use
//     std::vector<uint16_t> temp(data.size());
//
//     PredictivePreprocessing<16, uint16_t, UnitDelayPredictor, Mapper<16, uint16_t>> preprocessorEncode;
//     preprocessorEncode.encode(data, temp);
//     PredictivePreprocessing<16, uint16_t, UnitDelayPredictor, Mapper<16, uint16_t>> preprocessorDecode;
//     preprocessorDecode.decode(temp, restored);
//
//     for (size_t i = 0; i < data.size(); ++i) {
//         std::cout
//             << "Original: " << data[i]
//             << ", Encoded: " << temp[i]
//             << ", Restored: " << restored[i]
//             << std::endl;
//     }
//     for (size_t i = 0; i < data.size(); ++i) {
//         if (data[i] != restored[i]) {
//             std::cerr << "Error at index " << i << ": original " << data[i] << ", restored " << restored[i] << std::endl;
//             assert(false);
//         }
//     }
//
// }


void checkUnsignedPreprocessor2() {
    constexpr size_t bpv = 16;
    using ValueType = uint16_t;
    std::vector<ValueType> data = {24545, 52202, 62306, 12021, 47971, 51097, 39233, 39115, 10224, 29218};
    // std::vector<ValueType> data = {47971, 51097, 39233, 39115, 10224, 29218};
    // std::vector<ValueType> data = {47971, 32344, 12521, 12309, 65111, 12000};
    std::vector<ValueType> restored = data; // For potential future use
    std::vector<ValueType> temp(data.size());
    ValueType reference = data[0];

    PredictivePreprocessingEncodeFast<ValueType, ValueType> preprocessorEncode(bpv);
    preprocessorEncode.encode(data, temp);

    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << "Data[" << i << "] = " << data[i]
                  << ", Encoded = " << temp[i] << std::endl;
    }

    PredictivePreprocessingDecodeFast<ValueType, ValueType> preprocessorDecode(bpv);
    preprocessorDecode.decode(temp, reference, restored);

    for (size_t i = 0; i < data.size(); ++i) {
        std::cout
            << "Original: " << data[i]
            << ", Encoded: " << temp[i]
            << ", Restored: " << restored[i]
            << std::endl;
    }
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] != restored[i]) {
            std::cerr << "Error at index " << i << ": original " << data[i] << ", restored " << restored[i] << std::endl;
            assert(false);
        }
    }

}

int main() {
    // checkSignedPreprocessor();
    checkUnsignedPreprocessor2();

    return 0;
}
