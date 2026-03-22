#include <cassert>
#include <vector>
#include <span>
#include <iostream>

#include "RsiArray.h"

int main() {
    {
        std::vector<uint16_t> data(134);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<uint16_t>(i);
        }

        // Buffer(size_t rsi, size_t blockSize, T* data, size_t size) : rsiSize_(rsi), blockSize_(blockSize), data_(data) {}
        RsiArray<uint16_t> buffer(2, 8, std::span<uint16_t>(data.data(), data.size()));
        // Buffer(size_t rsi, size_t blockSize, T* data, size_t size) : rsiSize_(rsi), blockSize_(blockSize), data_(data) {}

        size_t rsiCount = 0;
        std::cout << "Running RSI Buffer Test:" << std::endl;
        for (auto rsi : buffer) {
            std::cout << "RSI Block: " << rsiCount++ << " size: " << rsi.size() << std::endl;
            size_t blockCount = 0;
            for (auto block : rsi) {
                std::cout << "Block " << blockCount++ << ": " << std::endl;
                size_t elementCount = 0;
                for (auto element : block) {
                    std::cout << "  Element " << elementCount++ << ": " << element << std::endl;
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        // Rsi<uint16_t> rsi(std::span<uint16_t>(data.data(), data.size()), 16);
        // size_t count = 0;
        // for (auto block : rsi) {
        //     for (size_t j = 0; j < block.size(); ++j) {
        //         assert(block[j] == static_cast<uint16_t>(count * 16 + j));
        //     }
        //     count++;
        // }
        // assert(count == 4);
    }

    return 0;
}
