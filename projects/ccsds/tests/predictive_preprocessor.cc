#include <cstddef>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>

#include "ccsds/Preprocessor.h"

void checkUnsignedPreprocessor2()
{
    constexpr size_t bpv = 16;
    using ValueType = uint16_t;
    std::vector<ValueType> data = { 24545, 52202, 62306, 12021, 47971, 51097, 39233, 39115, 10224, 29218 };
    std::vector<ValueType> restored = data;  // For potential future use
    std::vector<ValueType> temp(data.size());
    ValueType reference = data[0];

    PredictivePreprocessingEncodeFast<ValueType, ValueType> preprocessorEncode(bpv);
    preprocessorEncode.encode(data, temp);

    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << "Data[" << i << "] = " << data[i]
                  << ", Encoded = " << temp[i] << '\n';
    }

    PredictivePreprocessingDecodeFast<ValueType, ValueType> preprocessorDecode(bpv);
    preprocessorDecode.decode(temp, reference, restored);

    for (size_t i = 0; i < data.size(); ++i) {
        std::cout
            << "Original: " << data[i]
            << ", Encoded: " << temp[i]
            << ", Restored: " << restored[i]
            << '\n';
    }
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] != restored[i]) {
            std::cerr << "Error at index " << i << ": original " << data[i] << ", restored " << restored[i] << '\n';
            assert(false);
        }
    }
}

int main()
{
    // checkSignedPreprocessor();
    checkUnsignedPreprocessor2();

    return 0;
}
