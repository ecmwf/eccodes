#include "GribBitsAnyEndianStub.h"
#include "Accessor.h"

#include <cassert>

namespace eccodes::accessor {

// A mask with x least-significant bits set, possibly 0 or >=32 */
// -1UL is 1111111... in every bit in binary representation
#define BIT_MASK(x) \
    (((x) == max_nbits) ? (unsigned long)-1UL : (1UL << (x)) - 1)

// decode a value consisting of nbits from an octet-bitstream to long-representation
unsigned long gribDecodeUnsignedLong(DataPointer input, long& bitPos, long numBits)
{
    assert(false); // TODO
    return 0;
}


}
