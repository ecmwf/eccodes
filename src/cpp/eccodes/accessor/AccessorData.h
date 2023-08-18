#ifndef _ACCESSOR_DATA_H_20230803_
#define _ACCESSOR_DATA_H_20230803_

#include "AccessorDefs.h"
#include "AccessorBuffer.h"

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
    virtual int nativeType() const;
    virtual void updateSize(std::size_t s);
    virtual std::size_t preferredSize(int fromHandle) const;
    virtual void resize(size_t newSize);
    virtual double nearestSmallerValue(double val) const;
    virtual bool compare(AccessorData const& rhs);
    virtual bool isMissing() const;

    // Pack support
    virtual bool pack(std::vector<long> const& values);
    virtual bool pack(std::vector<double> const& values);
    virtual bool pack(std::vector<float> const& values);
    virtual bool pack(std::vector<char> const& values);
    virtual bool pack(std::vector<StringArray> const& values);
    virtual bool pack(std::vector<std::byte> const& values);
    virtual bool pack(grib_expression const& expression);
    virtual bool packMissing();

    // Unpack support
    virtual bool unpack(std::vector<long> &values) const;
    virtual bool unpack(std::vector<double> &values) const;
    virtual bool unpack(std::vector<float> &values) const;
    virtual bool unpack(std::vector<char> &values) const;
    virtual bool unpack(std::vector<StringArray> &values) const;
    virtual bool unpack(std::vector<std::byte> &values) const;
    virtual bool unpackElement(std::size_t index, double& val) const;
    virtual bool unpackElement(std::size_t index, float& val) const;
    virtual bool unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const;
    virtual bool unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const;
    virtual bool unpackSubarray(std::vector<double> &values, std::size_t start) const;

private:
    AccessorBuffer buffer_{};
    long length_{};
    long offset_{};
    unsigned long flags_{};
    int dirty_{};
    std::unique_ptr<grib_virtual_value> vvalue_{};
    std::string set_{};
};

}

#endif // _ACCESSOR_DATA_H_20230803_
