#ifndef _ACCESSOR_H_20230630_
#define _ACCESSOR_H_20230630_

#include "AccessorData.h"
#include <memory>
#include <string>
#include <vector>

struct grib_context;
struct grib_handle;
struct grib_action;
struct grib_section;
struct grib_dumper;
struct grib_expression;
struct grib_arguments;

namespace eccodes::accessor {

class Accessor;
using AccessorPtr = std::shared_ptr<Accessor>;
using stringArray = std::vector<std::string>;

class Accessor
{
public:
    Accessor(std::unique_ptr<AccessorData> data, grib_section* p, grib_action* creator);

    void dump(grib_dumper const& dumper) const;
    long nextOffset() const;
    std::size_t stringLength() const;
    int valueCount(long& count) const;
    long byteCount() const;
    long byteOffset() const;
    int nativeType() const;
    grib_section* subSection() const;
    int notify_change(AccessorPtr const observed) const;
    void updateSize(std::size_t s) const;
    std::size_t preferredSize(int fromHandle) const;
    void resize(size_t newSize) const;
    int nearestSmallerValue(double val, double& nearest) const;
    AccessorPtr next(int mod);
    int compare(AccessorPtr const rhs) const;
    int packMissing() const;
    int isMissing() const;
    int pack(grib_expression const& expression);
    int unpackSubarray(std::vector<double> &values, std::size_t start) const;

    template<typename T>
    int pack(std::vector<T> const& values);

    template<typename T>
    int unpack(std::vector<T> &values) const;

    template<typename T>
    int unpackElement(std::size_t index, T& val) const;

    template<typename T>
    int unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<T> &valArray) const;

private:
    std::unique_ptr<AccessorData> data_{};
    std::string name_{};
    std::string nameSpace_{};
    grib_context* context_{};   // Conversion helper - will change later...
    grib_handle* handle_{};     // Conversion helper - will change later...
    grib_action* creator_{};    // Conversion helper - will change later...
    std::weak_ptr<Accessor> parent_{};
    std::weak_ptr<Accessor> next_{};
    std::weak_ptr<Accessor> previous_{};
    grib_section* subSection_{}; // Conversion helper - will change later...
    std::vector<std::string> allNames_{};
    std::vector<std::string> allNameSpaces_{};
    bool dirty_{};
    std::vector<std::weak_ptr<Accessor>> same_;
    long loop_;
    std::vector<std::weak_ptr<Accessor>> attributes_;
    std::vector<std::weak_ptr<Accessor>> parentAsAttribute_;
};

template<typename T>
int Accessor::pack(std::vector<T> const& values)
{
    return data_->pack(values);
}

template<typename T>
int Accessor::unpack(std::vector<T> &values) const
{
    return data_->unpack(values);
}

template<typename T>
int Accessor::unpackElement(std::size_t index, T& val) const
{
    return data_->unpackElement(index, val);
}

template<typename T>
int Accessor::unpackElementSet(std::vector<std::size_t> const& indexArray, std::vector<T> &valArray) const
{
    return data_->unpackElementSet(indexArray, valArray);
}

}

#endif // _ACCESSOR_H_20230630_
