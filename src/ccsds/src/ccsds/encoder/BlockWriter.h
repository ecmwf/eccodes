#pragma once

#include <cstdint>
#include <cstddef>
#include <variant>
#include <array>

#include "BitBuffer.h"
#include "Span.h"


namespace encoder {

// NC
template <typename ValueType>
struct NCArgs {
    Span<ValueType> values;
};

// SE
template <typename ValueType>
struct SEArgs {
    ValueType reference;
    std::array<ValueType, 32> encodedValues;
};

// SS
template <typename ValueType>
struct SSArgs
{
    uint8_t k;
    Span<ValueType> values;
};

// ZB
template <typename ValueType>
struct ZBArgs {
    ValueType reference;
    size_t nBlocks;
};

template <typename ValueType>
using Args = std::variant<
    NCArgs<ValueType>,
    SEArgs<ValueType>,
    SSArgs<ValueType>,
    ZBArgs<ValueType>
>;

template <typename ValueType>
struct BlockWriter {
    size_t size;
    size_t offset;
    void* self;
    Args<ValueType> args;
    size_t (*writeTo)(const void* self, const Args<ValueType>& args, size_t offset, BitBuffer& buffer);

    inline size_t write(BitBuffer& buffer) {
        return writeTo(self, args, offset, buffer);
    }
};

} // namespace writer
