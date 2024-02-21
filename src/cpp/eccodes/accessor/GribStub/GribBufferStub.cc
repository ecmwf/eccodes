#include "GribBufferStub.h"
#include "Accessor.h"

#include <cassert>

namespace eccodes::accessor {

void griBufferDelete(AccessorDataBuffer& b)
{
    assert(false);
}

void gribBufferReplace(const AccessorDataBuffer& data, size_t newsize, int update_lengths, int update_paddings)
{
    assert(false);
}

void gribBufferSetUlengthBits(AccessorDataBuffer& b, size_t length_bits)
{
    assert(false);
}

AccessorDataBuffer gribCreateGrowableBuffer()
{
    assert(false);

    return {};
}



}
