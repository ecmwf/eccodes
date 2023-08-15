#ifndef _ACCESSOR_DATA_H_20230803_
#define _ACCESSOR_DATA_H_20230803_

#include <string>
#include <memory>
#include <vector>

struct grib_virtual_value;
struct grib_expression;
struct grib_dumper;
struct grib_arguments;

namespace eccodes::accessor {

using stringArray = std::vector<std::string>;

class AccessorData
{
public:
    AccessorData(const long len, grib_arguments* arg);
    virtual ~AccessorData() = 0;

    void dump(grib_dumper const& dumper) const;
    std::size_t stringLength() const;
    int valueCount(long& count) const;
    long byteCount() const;
    long byteOffset() const;
    int nativeType() const;
    void updateSize(std::size_t s);
    std::size_t preferredSize(int fromHandle) const;
    void resize(size_t newSize);
    int nearestSmallerValue(double val, double& nearest) const;
    int compare(AccessorData const& rhs);
    int packMissing();
    int isMissing() const;

    int pack(std::vector<long> const& values);
    int unpack(std::vector<long> &values) const;
    int pack(std::vector<double> const& values);
    int unpack(std::vector<double> &values) const;
    int pack(std::vector<float> const& values);
    int unpack(std::vector<float> &values) const;
    int pack(std::vector<std::string> const& values);
    int unpack(std::vector<std::string> &values) const;
    int pack(std::vector<stringArray> const& values);
    int unpack(std::vector<stringArray> &values) const;
    int pack(std::vector<std::byte> const& values);
    int unpack(std::vector<std::byte> &values) const;
    int pack(grib_expression const& expression);
    int unpackElement(std::size_t index, double& val) const;
    int unpackElement(std::size_t index, float& val) const;
    int unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const;
    int unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const;
    int unpackSubarray(std::vector<double> &values, std::size_t start) const;

private:
    long length_{};
    long offset_{};
    unsigned long flags_{};
    int dirty_{};
    std::unique_ptr<grib_virtual_value> vvalue_{};
    std::string set_{};

    virtual void dumpImpl(grib_dumper const& dumper) const;
    virtual std::size_t stringLengthImpl() const;
    virtual int valueCountImpl(long& count) const;
    virtual long byteCountImpl() const;
    virtual long byteOffsetImpl() const;
    virtual int nativeTypeImpl() const;
    virtual void updateSizeImpl(std::size_t s);
    virtual std::size_t preferredSizeImpl(int fromHandle) const;
    virtual void resizeImpl(size_t newSize);
    virtual int nearestSmallerValueImpl(double val, double& nearest) const;
    virtual int compareImpl(AccessorData const& rhs);
    virtual int packMissingImpl();
    virtual int isMissingImpl() const;

    virtual int packImpl(std::vector<long> const& values);
    virtual int unpackImpl(std::vector<long> &values) const;
    virtual int packImpl(std::vector<double> const& values);
    virtual int unpackImpl(std::vector<double> &values) const;
    virtual int packImpl(std::vector<float> const& values);
    virtual int unpackImpl(std::vector<float> &values) const;
    virtual int packImpl(std::vector<std::string> const& values);
    virtual int unpackImpl(std::vector<std::string> &values) const;
    virtual int packImpl(std::vector<stringArray> const& values);
    virtual int unpackImpl(std::vector<stringArray> &values) const;
    virtual int packImpl(std::vector<std::byte> const& values);
    virtual int unpackImpl(std::vector<std::byte> &values) const;
    virtual int packImpl(grib_expression const& expression);
    virtual int unpackElementImpl(std::size_t index, double& val) const;
    virtual int unpackElementImpl(std::size_t index, float& val) const;
    virtual int unpackElementSetImpl(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const;
    virtual int unpackElementSetImpl(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const;
    virtual int unpackSubarrayImpl(std::vector<double> &values, std::size_t start) const;
};

}

#endif // _ACCESSOR_DATA_H_20230803_
