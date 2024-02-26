#include "GribBitsAnyEndianStub.h"
#include "Accessor.h"

#include <cassert>

namespace eccodes::accessor {

unsigned long gribDecodeUnsignedLong(const AccessorDataPointer input, long& bitPos, long numBits)
{
    return grib_decode_unsigned_long(input, &bitPos, numBits);
}

unsigned long gribDecodeUnsignedLong(const AccessorDataBuffer& input, long& bitPos, long numBits)
{
    return grib_decode_unsigned_long(input.data(), &bitPos, numBits);
}

GribStatus gribEncodeUnsignedLong(AccessorDataPointer p, unsigned long val, long& bitPos, long numBits)
{
    GribStatus status = GribStatus::SUCCESS;

    if (grib_encode_unsigned_long(p, val, &bitPos, numBits) != GRIB_SUCCESS) {
        status = GribStatus::ENCODING_ERROR;
    }

    return status;
}

GribStatus gribEncodeUnsignedLong(AccessorDataBuffer& p, unsigned long val, long& bitPos, long numBits)
{
    GribStatus status = GribStatus::SUCCESS;

    if (grib_encode_unsigned_long(p.data(), val, &bitPos, numBits) != GRIB_SUCCESS) {
        status = GribStatus::ENCODING_ERROR;
    }

    return status;
}

GribStatus gribEncodeUnsignedLongb(AccessorDataPointer p, unsigned long val, long& bitPos, long nnumBitsb)
{
    assert(false); // TODO
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribDecodeLongArray(const AccessorDataPointer p, long& bitp, long bitsPerValue, size_t nVals, std::vector<long>& val)
{
    assert(false); // TODO
    return GribStatus::NOT_IMPLEMENTED;
}

}
