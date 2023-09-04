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
    return data_->valueCount();
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

bool Accessor::newBuffer(AccessorBuffer const& accBuffer)
{
    return data_->newBuffer(accBuffer);
}

AccessorBuffer Accessor::currentBuffer() const
{
    return data_->currentBuffer();
}

GribStatus Accessor::packMissing() const
{
    return data_->packMissing();
}

std::vector<double> Accessor::unpackSubarray(std::size_t start) const
{
    std::vector<double> values{};
    return data_->unpackSubarray(values, start) == GribStatus::SUCCESS ? values : std::vector<double>{};
}

}

