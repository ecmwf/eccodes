#include "Accessor.h"

namespace eccodes {

grib_accessor* asGribAccessor(Accessor const& accessor)
{
    return accessor.pimpl->operator grib_accessor*();
}

void accessorDump(Accessor const& accessor, grib_dumper* dumper)
{
    return accessor.pimpl->accessorDump(dumper);
}

long accessorNextOffset(Accessor const& accessor)
{
    return accessor.pimpl->accessorNextOffset();
}

size_t accessorStringLength(Accessor const& accessor)
{
    return accessor.pimpl->accessorStringLength();
}

int accessorValueCount(Accessor const& accessor, long* count)
{
    return accessor.pimpl->accessorValueCount(count);
}

long accessorByteCount(Accessor const& accessor)
{
    return accessor.pimpl->accessorByteCount();
}

long accessorByteOffset(Accessor const& accessor)
{
    return accessor.pimpl->accessorByteOffset();
}

int accessorNativeType(Accessor const& accessor)
{
    return accessor.pimpl->accessorNativeType();
}

grib_section* accessorSubSection(Accessor const& accessor)
{
    return accessor.pimpl->accessorSubSection();
}

int accessorPackMissing(Accessor const& accessor)
{
    return accessor.pimpl->accessorPackMissing();
}

int accessorIsMissing(Accessor const& accessor)
{
    return accessor.pimpl->accessorIsMissing();
}

int accessorPackLong(Accessor const& accessor, const long* val, size_t* len)
{
    return accessor.pimpl->accessorPackLong(val, len);
}

int accessorUnpackLong(Accessor const& accessor, long* val, size_t* len)
{
    return accessor.pimpl->accessorUnpackLong(val, len);
}

int accessorPackDouble(Accessor const& accessor, const double* val, size_t* len)
{
    return accessor.pimpl->accessorPackDouble(val, len);
}

int accessorPackFloat(Accessor const& accessor, const float* val, size_t* len)
{
    return accessor.pimpl->accessorPackFloat(val, len);
}

int accessorUnpackDouble(Accessor const& accessor, double* val, size_t* len)
{
    return accessor.pimpl->accessorUnpackDouble(val, len);
}

int accessorUnpackFloat(Accessor const& accessor, float* val, size_t* len)
{
    return accessor.pimpl->accessorUnpackFloat(val, len);
}

int accessorPackString(Accessor const& accessor, const char* v, size_t* len)
{
    return accessor.pimpl->accessorPackString(v, len);
}

int accessorUnpackString(Accessor const& accessor, char* v, size_t* len)
{
    return accessor.pimpl->accessorUnpackString(v, len);
}

int accessorPackStringArray(Accessor const& accessor, const char** v, size_t* len)
{
    return accessor.pimpl->accessorPackStringArray(v, len);
}

int accessorUnpackStringArray(Accessor const& accessor, char** v, size_t* len)
{
    return accessor.pimpl->accessorUnpackStringArray(v, len);
}

int accessorPackBytes(Accessor const& accessor, const unsigned char* val, size_t* len)
{
    return accessor.pimpl->accessorPackBytes(val, len);
}

int accessorUnpackBytes(Accessor const& accessor, unsigned char* val, size_t* len)
{
    return accessor.pimpl->accessorUnpackBytes(val, len);
}

int accessorPackExpression(Accessor const& accessor, grib_expression* e)
{
    return accessor.pimpl->accessorPackExpression(e);
}

int accessorNotifyChange(Accessor const& accessor, grib_accessor* observed)
{
    return accessor.pimpl->accessorNotifyChange(observed);
}

void accessorUpdateSize(Accessor const& accessor, size_t s)
{
    return accessor.pimpl->accessorUpdateSize(s);
}

size_t accessorPreferredSize(Accessor const& accessor, int from_handle)
{
    return accessor.pimpl->accessorPreferredSize(from_handle);
}

void accessorResize(Accessor const& accessor, size_t new_size)
{
    return accessor.pimpl->accessorResize(new_size);
}

int accessorNearestSmallerValue(Accessor const& accessor, double val, double* nearest)
{
    return accessor.pimpl->accessorNearestSmallerValue(val, nearest);
}

grib_accessor* accessorNext(Accessor const& accessor, int mod)
{
    return accessor.pimpl->accessorNext(mod);
}

int accessorCompare(Accessor const& accessor, grib_accessor* b)
{
    return accessor.pimpl->accessorCompare(b);
}

int accessorUnpackDoubleElement(Accessor const& accessor, size_t i, double* val)
{
    return accessor.pimpl->accessorUnpackDoubleElement(i, val);
}

int accessorUnpackFloatElement(Accessor const& accessor, size_t i, float* val)
{
    return accessor.pimpl->accessorUnpackFloatElement(i, val);
}

int accessorUnpackDoubleElementSet(Accessor const& accessor, const size_t* index_array, size_t len, double* val_array)
{
    return accessor.pimpl->accessorUnpackDoubleElementSet(index_array, len, val_array);
}

int accessorUnpackFloatElementSet(Accessor const& accessor, const size_t* index_array, size_t len, float* val_array)
{
    return accessor.pimpl->accessorUnpackFloatElementSet(index_array, len, val_array);
}

int accessorUnpackDoubleSubarray(Accessor const& accessor, double* val, size_t start, size_t len)
{
    return accessor.pimpl->accessorUnpackDoubleSubarray(val, start, len);
}

int accessorClear(Accessor const& accessor)
{
    return accessor.pimpl->accessorClear();
}

grib_accessor* accessorMakeClone(Accessor const& accessor, grib_section* s, int* err)
{
    return accessor.pimpl->accessorMakeClone(s, err);
}

}

