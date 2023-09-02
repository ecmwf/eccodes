#pragma once

#include "AccessorDefs.h"
#include "AccessorBuffer.h"
#include "AccessorData/AccessorData.h"
#include "AccessorUtils/GribType.h"
#include "AccessorUtils/GribStatus.h"
#include <memory>
#include <string>
#include <vector>

struct grib_expression;

namespace eccodes::accessor {

class Accessor
{
public:
    Accessor(AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorDataPtr data);

    AccessorName name() const;

    //void dump(grib_dumper const& dumper) const;
    std::size_t stringLength() const;
    long valueCount() const;
    GribType nativeType() const;
    double nearestSmallerValue(double val) const;
    int compare(AccessorPtr const rhs) const;
    int isMissing() const;

    bool newBuffer(AccessorBuffer const& accBuffer);
    AccessorBuffer currentBuffer() const;

    // Pack support
    template<typename T>
    GribStatus pack(std::vector<T> const& values);

    GribStatus pack(grib_expression const& expression);
    GribStatus packMissing() const;

    // Unpack support
    template<typename T>
    std::vector<T> unpack() const;

    // Support for user supplied buffer, which MUST be the correct size!
    template<typename T>
    GribStatus unpack(std::vector<T>& values) const;

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, T>
    unpackElement(std::size_t index) const;

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, std::vector<T>>
    unpackElementSet(std::vector<std::size_t> const& indexArray) const;

    std::vector<double> unpackSubarray(std::size_t start) const;

private:
    AccessorDataPtr data_{};
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
GribStatus Accessor::pack(std::vector<T> const& values)
{
    return data_->pack(values);
}

// Unpack support
template<typename T>
std::vector<T> Accessor::unpack() const
{
    std::vector<T> values;
    return data_->unpack(values) == GribStatus::SUCCESS ? values : std::vector<T>{};
}

template<typename T>
GribStatus Accessor::unpack(std::vector<T>& values) const
{
    if(auto tempValues = unpack<T>(); tempValues.size() <= values.size())
    {
        values = std::move(tempValues);
        return GribStatus::SUCCESS;
    }
    return GribStatus::BUFFER_TOO_SMALL;
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
