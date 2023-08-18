#include "AccessorData.h"

#include "grib_api_internal.h"

// This file implements the public API for AccessorData
//
// The private implementation has been moved to AccessorDataImpl.cc for clarity

namespace eccodes::accessor {

AccessorData::AccessorData(AccessorInitData const& initData)
{
    // TO DO - FULL INIT()
}

AccessorData::~AccessorData() = default;

bool AccessorData::newBuffer(AccessorBuffer const& accBuffer)
{
    buffer_ = accBuffer;
    return true;
}

AccessorBuffer AccessorData::currentBuffer() const
{
    return buffer_;
}

std::size_t AccessorData::stringLength() const
{
    return 1024;
}

long AccessorData::valueCount() const
{
    return 1;
}

long AccessorData::byteCount() const
{
    return length_;
}

long AccessorData::byteOffset() const
{
    return offset_;
}

int AccessorData::nativeType() const
{
    return GRIB_TYPE_UNDEFINED;
}

void AccessorData::updateSize(std::size_t s)
{
    Assert(0);
}

std::size_t AccessorData::preferredSize(int fromHandle) const
{
    return length_;
}

void AccessorData::resize(size_t newSize)
{
    Assert(0);
}

double AccessorData::nearestSmallerValue(double val) const
{
    Assert(0);
    return 0.0;
}

bool AccessorData::compare(AccessorData const& rhs)
{
    Assert(0);
    return false;
}

bool AccessorData::isMissing() const
{
    Assert(0); // TO DO
    return false;
}

// Pack support
bool AccessorData::pack(std::vector<long> const& values)
{
    return false;
}

bool AccessorData::pack(std::vector<double> const& values)
{
    return false;
}

bool AccessorData::pack(std::vector<float> const& values)
{
    return false;
}

bool AccessorData::pack(std::vector<char> const& values)
{
    return false;
}

bool AccessorData::pack(std::vector<StringArray> const& values)
{
    return false;
}

bool AccessorData::pack(std::vector<std::byte> const& values)
{
    return false;
}

bool AccessorData::pack(grib_expression const& expression)
{
    return false;
}

bool AccessorData::packMissing()
{
    return false;
}

// Unpack support
bool AccessorData::unpack(std::vector<long> &values) const
{
    return false;
}

bool AccessorData::unpack(std::vector<double> &values) const
{
    return false;
}

bool AccessorData::unpack(std::vector<float> &values) const
{
    return false;
}

bool AccessorData::unpack(std::vector<char> &values) const
{
    return false;
}

bool AccessorData::unpack(std::vector<StringArray> &values) const
{
    return false;
}

bool AccessorData::unpack(std::vector<std::byte> &values) const
{
    return false;
}

bool AccessorData::unpackElement(std::size_t index, double& val) const
{
    return false;
}

bool AccessorData::unpackElement(std::size_t index, float& val) const
{
    return false;
}

bool AccessorData::unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const
{
    return false;
}

bool AccessorData::unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const
{
    return false;
}

bool AccessorData::unpackSubarray(std::vector<double> &values, std::size_t start) const
{
    return false;
}

}
