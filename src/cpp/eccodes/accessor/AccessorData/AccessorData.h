#pragma once

#include "AccessorDefs.h"
#include "AccessorData/AccessorInitData.h"
#include "AccessorBuffer.h"
#include "AccessorTraits.h"
#include "AccessorUtils/GribType.h"
#include "AccessorUtils/GribStatus.h"

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

    bool newBuffer(AccessorBuffer const& accBuffer);
    AccessorBuffer currentBuffer() const;

    //void dump(grib_dumper const& dumper) const;
    virtual std::size_t stringLength() const;
    virtual long valueCount() const;
    virtual long byteCount() const;
    virtual long byteOffset() const;
    virtual GribType nativeType() const;
    virtual void updateSize(std::size_t s);
    virtual std::size_t preferredSize(int fromHandle) const;
    virtual void resize(size_t newSize);
    virtual double nearestSmallerValue(double val) const;
    virtual bool compare(AccessorData const& rhs);
    virtual bool isMissing() const;

    // Pack - single value
    virtual GribStatus pack(long const& value);
    virtual GribStatus pack(double const& value);
    virtual GribStatus pack(float const& value);
    virtual GribStatus pack(grib_expression const& expression);
    virtual GribStatus packMissing();

    // Pack - buffer
    virtual GribStatus pack(std::string const& values);
    virtual GribStatus pack(std::vector<long> const& values);
    virtual GribStatus pack(std::vector<double> const& values);
    virtual GribStatus pack(std::vector<float> const& values);
    virtual GribStatus pack(std::vector<StringArray> const& values);
    virtual GribStatus pack(std::vector<std::byte> const& values);

    // Unpack - single value
    template<typename T>
    GribStatus unpack(T &value) const;

    // Unpack - buffer
    virtual GribStatus unpack(std::string &values) const;
    virtual GribStatus unpack(std::vector<long>& values) const;
    virtual GribStatus unpack(std::vector<double>& values) const;
    virtual GribStatus unpack(std::vector<float>& values) const;
    virtual GribStatus unpack(std::vector<StringArray> &values) const;
    virtual GribStatus unpack(std::vector<std::byte> &values) const;

    // Unpack - multiple values
    virtual GribStatus unpackElement(std::size_t index, double& val) const;
    virtual GribStatus unpackElement(std::size_t index, float& val) const;
    virtual GribStatus unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const;
    virtual GribStatus unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const;
    virtual GribStatus unpackSubarray(std::vector<double> &values, std::size_t start) const;

private:
    AccessorBuffer buffer_{};
    long length_{};
    long offset_{};
    unsigned long flags_{};
    int dirty_{};
    std::unique_ptr<grib_virtual_value> vvalue_{};
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
