#include "GribBitsAnyEndianStub.h"
#include "Accessor.h"

#include <cassert>

namespace eccodes::accessor {

unsigned long gribDecodeUnsignedLong(const AccessorDataPointer input, long& bitPos, long numBits)
{
    assert(false); // TODO
    return 0;
}

unsigned long gribDecodeUnsignedLong(const AccessorDataBuffer& input, long& bitPos, long numBits)
{
    assert(false); // TODO
    return 0;
}

GribStatus gribEncodeUnsignedLong(AccessorDataPointer p, unsigned long val, long& bitPos, long numBits)
{
    assert(false); // TODO
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribEncodeUnsignedLong(AccessorDataBuffer& p, unsigned long val, long& bitPos, long numBits)
{
    assert(false); // TODO
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribEncodeUnsignedLongb(AccessorDataPointer p, unsigned long val, long& bitPos, long nnumBitsb)
{
    assert(false); // TODO
    return GribStatus::NOT_IMPLEMENTED;
}

}
