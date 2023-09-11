#pragma once

#include "AccessorDefs.h"
#include "AccessorBuffer.h"
#include "AccessorTraits.h"
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

    AccessorName    name() const;
    std::size_t     stringLength() const;
    long            valueCount() const;
    GribType        nativeType() const;
    double          nearestSmallerValue(double val) const;
    int             compare(AccessorPtr const rhs) const;
    int             isMissing() const;

    bool            newBuffer(AccessorBuffer const& accBuffer);
    AccessorBuffer  currentBuffer() const;

    template<typename T>
    GribStatus pack(T const& values);
    GribStatus packMissing() const;

    template<typename T>
    T unpack() const;

    // Unpack to user-supplied buffer, which MUST be the correct size!
    template<typename T>
    GribStatus unpack(T& values) const;

    template<typename T>
    T unpackElement(std::size_t index) const;

    template<typename T>
    std::vector<T> unpackElementSet(std::vector<std::size_t> const& indexArray) const;

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
GribStatus Accessor::pack(T const& values)
{
    static_assert(isAllowedPackType<T>::value, "Unsupported pack() type supplied");

    return data_->pack(values);
}

// Unpack support
template<typename T>
T Accessor::unpack() const
{
    static_assert(isAllowedUnpackType<T>::value, "Unsupported unpack() type supplied");

    T values;
    return data_->unpack(values) == GribStatus::SUCCESS ? values : T{};
}

template<typename T>
GribStatus Accessor::unpack(T& values) const
{
    static_assert(isAllowedUnpackType<T>::value, "Unsupported unpack() type supplied");

    if(auto tempValues = unpack<T>(); tempValues.size() <= values.size())
    {
        values = std::move(tempValues);
        return GribStatus::SUCCESS;
    }
    return GribStatus::BUFFER_TOO_SMALL;
}

template<typename T>
T Accessor::unpackElement(std::size_t index) const
{
    static_assert(isAllowedUnpackElementType<T>::value, "Unsupported unpackElement() type supplied");

    T val{};
    return data_->unpackElement(index, val) ? val : T{};
}

template<typename T>
std::vector<T> Accessor::unpackElementSet(std::vector<std::size_t> const& indexArray) const
{
    static_assert(isAllowedUnpackElementType<T>::value, "Unsupported unpackElementSet() type supplied");

    std::vector<T> valArray;
    return data_->unpackElementSet(indexArray, valArray) ? valArray : std::vector<T>{};
}

}
