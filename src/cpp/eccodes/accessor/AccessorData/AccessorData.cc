#include "AccessorData.h"
#include "AccessorUtils/AccessorException.h"
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

bool AccessorData::newBuffer(AccessorDataView const& accBuffer)
{
    buffer_ = accBuffer;
    return true;
}

AccessorDataView AccessorData::currentBuffer() const
{
    return buffer_;
}

void AccessorData::dump() const
{
    throw AccessorException(GribStatus::NOT_IMPLEMENTED);
}

std::size_t AccessorData::stringLength() const
{
    return 1024;
}

GribStatus AccessorData::valueCount(long& count) const
{
    count = 1;
    return GribStatus::NOT_IMPLEMENTED;
}

long AccessorData::byteCount() const
{
    return length_;
}

long AccessorData::byteOffset() const
{
    return offset_;
}

GribType AccessorData::nativeType() const
{
    return GribType::UNDEFINED;
}

void AccessorData::updateSize(std::size_t s)
{
    throw AccessorException(GribStatus::NOT_IMPLEMENTED);
}

std::size_t AccessorData::preferredSize(int fromHandle) const
{
    return length_;
}

void AccessorData::resize(std::size_t newSize)
{
    throw AccessorException(GribStatus::NOT_IMPLEMENTED);
}

double AccessorData::nearestSmallerValue(double val) const
{
    throw AccessorException(GribStatus::NOT_IMPLEMENTED);
    return 0.0;
}

bool AccessorData::compare(AccessorData const& rhs) const
{
    throw AccessorException(GribStatus::NOT_IMPLEMENTED);
    return false;
}

bool AccessorData::isMissing() const
{
    throw AccessorException(GribStatus::NOT_IMPLEMENTED); // TO DO
    return false;
}

AccessorDataPtr AccessorData::clone() const
{
    throw AccessorException(GribStatus::NOT_IMPLEMENTED); // TO DO
    return nullptr;
}

// Pack - single value
GribStatus AccessorData::pack(long const& longValue)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(double const& doubleValue)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(float const& floatValue)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(grib_expression const& expression)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::packMissing()
{
    return GribStatus::NOT_IMPLEMENTED;
}

// Pack - buffer
GribStatus AccessorData::pack(std::string const& stringValue)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(std::vector<long> const& longValues)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(std::vector<double> const& doubleValues)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(std::vector<float> const& floatValues)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(std::vector<StringArray> const& stringValues)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::pack(std::vector<unsigned char> const& byteValues)
{
    return GribStatus::NOT_IMPLEMENTED;
}

// Unpack - buffer
GribStatus AccessorData::unpack(std::string &stringValue) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpack(std::vector<long> &longValues) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpack(std::vector<double> &doubleValues) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpack(std::vector<float> &floatValues) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpack(std::vector<StringArray> &stringValues) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpack(std::vector<unsigned char> &byteValues) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

// Unpack - multiple values
GribStatus AccessorData::unpackElement(std::size_t index, double& val) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpackElement(std::size_t index, float& val) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus AccessorData::unpackSubarray(std::vector<double> &values, std::size_t start) const
{
    return GribStatus::NOT_IMPLEMENTED;
}

}
