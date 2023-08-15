#include "AccessorData.h"

#include "grib_api_internal.h"

// This file implements the public API for AccessorData
//
// The private implementation has been moved to AccessorDataImpl.cc for clarity

namespace eccodes::accessor {

AccessorData::AccessorData(const long len, grib_arguments* arg)
{
    // TO DO - FULL INIT()
}

AccessorData::~AccessorData() = default;

void AccessorData::dump(grib_dumper const& dumper) const
{
    return dumpImpl(dumper);
}

std::size_t AccessorData::stringLength() const
{
    return stringLengthImpl();
}

int AccessorData::valueCount(long& count) const
{
    return valueCountImpl(count);
}

long AccessorData::byteCount() const
{
    return byteCountImpl();
}

long AccessorData::byteOffset() const
{
    return byteOffsetImpl();
}

int AccessorData::nativeType() const
{
    return nativeTypeImpl();
}

void AccessorData::updateSize(std::size_t s)
{
    return updateSizeImpl(s);
}

std::size_t AccessorData::preferredSize(int fromHandle) const
{
    return preferredSizeImpl(fromHandle);
}

void AccessorData::resize(size_t newSize)
{
    return resizeImpl(newSize);
}

int AccessorData::nearestSmallerValue(double val, double& nearest) const
{
    return nearestSmallerValueImpl(val, nearest);
}

int AccessorData::compare(AccessorData const& rhs)
{
    return compareImpl(rhs);
}

int AccessorData::packMissing()
{
    return packMissingImpl();
}

int AccessorData::isMissing() const
{
    return isMissingImpl();
}

int AccessorData::pack(std::vector<long> const& values)
{
    return packImpl(values);
}

int AccessorData::unpack(std::vector<long> &values) const
{
    return unpackImpl(values);
}

int AccessorData::pack(std::vector<double> const& values)
{
    return packImpl(values);
}

int AccessorData::unpack(std::vector<double> &values) const
{
    return unpackImpl(values);
}

int AccessorData::pack(std::vector<float> const& values)
{
    return packImpl(values);
}

int AccessorData::unpack(std::vector<float> &values) const
{
    return unpackImpl(values);
}

int AccessorData::pack(std::vector<std::string> const& values)
{
    return packImpl(values);
}

int AccessorData::unpack(std::vector<std::string> &values) const
{
    return unpackImpl(values);
}

int AccessorData::pack(std::vector<stringArray> const& values)
{
    return packImpl(values);
}

int AccessorData::unpack(std::vector<stringArray> &values) const
{
    return unpackImpl(values);
}

int AccessorData::pack(std::vector<std::byte> const& values)
{
    return packImpl(values);
}

int AccessorData::unpack(std::vector<std::byte> &values) const
{
    return unpackImpl(values);
}

int AccessorData::pack(grib_expression const& expression)
{
    return packImpl(expression);
}

int AccessorData::unpackElement(std::size_t index, double& val) const
{
    return unpackElementImpl(index, val);
}

int AccessorData::unpackElement(std::size_t index, float& val) const
{
    return unpackElementImpl(index, val);
}

int AccessorData::unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const
{
    return unpackElementSetImpl(indexArray, valArray);
}

int AccessorData::unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const
{
    return unpackElementSetImpl(indexArray, valArray);
}

int AccessorData::unpackSubarray(std::vector<double> &values, std::size_t start) const
{
    return unpackSubarrayImpl(values, start);
}

}
