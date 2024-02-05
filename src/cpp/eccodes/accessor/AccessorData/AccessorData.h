#pragma once

#include "AccessorDefs.h"
#include "AccessorData/AccessorInitData.h"
#include "AccessorBuffer.h"
#include "AccessorTraits.h"
#include "GribCpp/GribType.h"
#include "GribCpp/GribStatus.h"
#include "GribStub/GribVirtualValueStub.h"

#include <string>
#include <memory>
#include <vector>

struct grib_virtual_value;
struct grib_expression;

namespace eccodes::accessor {

class AccessorData
{
public:
    AccessorData(AccessorInitData const& initData);
    virtual ~AccessorData() = 0;

    bool newBuffer(AccessorDataView const& accBuffer);
    AccessorDataView currentBuffer() const;

    virtual void dump() const;
    virtual std::size_t stringLength() const;
    //virtual long valueCount() const;
    virtual GribStatus valueCount(long& count) const;
    virtual long byteCount() const;
    virtual long byteOffset() const;
    virtual GribType nativeType() const;
    virtual void updateSize(std::size_t s);
    virtual std::size_t preferredSize(int fromHandle) const;
    virtual void resize(std::size_t newSize);
    virtual double nearestSmallerValue(double val) const;
    virtual bool compare(AccessorData const& rhs) const;
    virtual bool isMissing() const;
    virtual AccessorDataPtr clone() const;

    // Pack - single value
    virtual GribStatus pack(long const& longValue);
    virtual GribStatus pack(double const& doubleValue);
    virtual GribStatus pack(float const& floatValue);
    virtual GribStatus pack(grib_expression const& expression);
    virtual GribStatus packMissing();

    // Pack - buffer
    virtual GribStatus pack(std::string const& stringValue);
    virtual GribStatus pack(std::vector<long> const& longValues);
    virtual GribStatus pack(std::vector<double> const& doubleValues);
    virtual GribStatus pack(std::vector<float> const& floatValues);
    virtual GribStatus pack(std::vector<StringArray> const& stringValues);
    virtual GribStatus pack(std::vector<unsigned char> const& byteValues);

    // Unpack - single value
    template<typename T>
    GribStatus unpack(T &value) const;

    // Unpack - buffer
    virtual GribStatus unpack(std::string &stringValue) const;
    virtual GribStatus unpack(std::vector<long>& longValues) const;
    virtual GribStatus unpack(std::vector<double>& doubleValues) const;
    virtual GribStatus unpack(std::vector<float>& floatValues) const;
    virtual GribStatus unpack(std::vector<StringArray> &stringValues) const;
    virtual GribStatus unpack(std::vector<unsigned char> &byteValues) const;

    // Unpack - multiple values
    virtual GribStatus unpackElement(std::size_t index, double& val) const;
    virtual GribStatus unpackElement(std::size_t index, float& val) const;
    virtual GribStatus unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const;
    virtual GribStatus unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const;
    virtual GribStatus unpackSubArray(std::vector<double> &values, std::size_t start) const;

    // Conversion helpers...
    AccessorLoaderPtr loader() const { return nullptr; }

// Ideally these would be private, but that makes the conversion much harder so they are protected instead
// This will be revisited later...
protected:
    AccessorDataView buffer_{};
    std::string name_{};
    long length_{};
    long offset_{};
    unsigned long flags_{};
    int dirty_{};
    GribVirtualValuePtr vvalue_{};
    std::string set_{};
};

template<typename T>
GribStatus AccessorData::unpack(T &value) const
{
    static_assert(isAllowedSimpleType<T>::value, "Unsupported unpack() type supplied");

    std::vector<T> values;
    unpack(values);

    if(values.size() == 1) {
        value = values[0];
        return GribStatus::SUCCESS; 
    }

    return GribStatus::COUNT_MISMATCH;
}

}
