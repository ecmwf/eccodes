#include "Accessor.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

Accessor::Accessor(AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorDataPtr data)
    : data_{std::move(data)}
    , name_{name}
    , nameSpace_{nameSpace}
    , allNames_{{name_}}
    , allNameSpaces_{{nameSpace_}}
{
    // TODO
    // Finish "Gen" Init;
}

AccessorName Accessor::name() const
{
    return name_;
}

std::size_t Accessor::stringLength() const
{
    return data_->stringLength();
}

long Accessor::valueCount() const
{
    long count{};
    data_->valueCount(count);
    return count;
}

GribType Accessor::nativeType() const
{
    return data_->nativeType();
}

double Accessor::nearestSmallerValue(double val) const
{
    return data_->nearestSmallerValue(val);
}

int Accessor::compare(AccessorPtr const rhs) const
{
    return data_->compare(*rhs->data_);
}

int Accessor::isMissing() const
{
    return data_->isMissing();
}

bool Accessor::newBuffer(AccessorDataView const& accBuffer)
{
    return data_->newBuffer(accBuffer);
}

AccessorDataView Accessor::currentBuffer() const
{
    return data_->currentBuffer();
}

GribStatus Accessor::packMissing() const
{
    return data_->packMissing();
}

std::vector<double> Accessor::unpackSubArray(std::size_t start) const
{
    std::vector<double> values{};
    return data_->unpackSubArray(values, start) == GribStatus::SUCCESS ? values : std::vector<double>{};
}

// Conversion support
long Accessor::byteCount() const
{
    return data_->byteCount();
}

long Accessor::byteOffset() const
{
    return data_->byteOffset();
}

}

