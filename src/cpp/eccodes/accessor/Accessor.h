#ifndef _ACCESSOR_H_20230630_
#define _ACCESSOR_H_20230630_

#include "AccessorDefs.h"
#include "AccessorData.h"
#include <memory>
#include <string>
#include <vector>

struct grib_expression;

namespace eccodes::accessor {

AccessorPtr makeAccessor(AccessorType const& type, AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData);

class Accessor
{
public:
    Accessor(AccessorName const& name, AccessorNameSpace const& nameSpace, std::unique_ptr<AccessorData> data);

    AccessorName name() const;

    //void dump(grib_dumper const& dumper) const;
    std::size_t stringLength() const;
    long valueCount() const;
    int nativeType() const;
    double nearestSmallerValue(double val) const;
    int compare(AccessorPtr const rhs) const;
    int isMissing() const;

    bool newBuffer(AccessorBuffer const& accBuffer);
    AccessorBuffer currentBuffer() const;

    // Pack support
    template<typename T>
    bool pack(std::vector<T> const& values);

    bool pack(grib_expression const& expression);
    bool packMissing() const;

    // Unpack support
    template<typename T>
    std::vector<T> unpack() const;

    template<typename T>
    bool unpack(std::vector<T>& values) const;

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, T>
    unpackElement(std::size_t index) const;

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, std::vector<T>>
    unpackElementSet(std::vector<std::size_t> const& indexArray) const;

    std::vector<double> unpackSubarray(std::size_t start) const;

private:
    std::unique_ptr<AccessorData> data_{};
    AccessorName name_;
    AccessorNameSpace nameSpace_;
    std::vector<AccessorName> allNames_{};
    std::vector<AccessorNameSpace> allNameSpaces_{};
    bool dirty_{};
    long loop_;
    std::vector<std::weak_ptr<Accessor>> attributes_;
    std::vector<std::weak_ptr<Accessor>> parentAsAttribute_;
};

// Pack support
template<typename T>
bool Accessor::pack(std::vector<T> const& values)
{
    return data_->pack(values);
}

// Unpack support
template<typename T>
std::vector<T> Accessor::unpack() const
{
    std::vector<T> values;
    return data_->unpack(values) ? values : std::vector<T>{};
}

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, T>
Accessor::unpackElement(std::size_t index) const
{
    T val{};
    return data_->unpackElement(index, val) ? val : T{};
}

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, std::vector<T>>
Accessor::unpackElementSet(std::vector<std::size_t> const& indexArray) const
{
    std::vector<T> valArray;
    return data_->unpackElementSet(indexArray, valArray) ? valArray : std::vector<T>{};
}

}

#endif // _ACCESSOR_H_20230630_
