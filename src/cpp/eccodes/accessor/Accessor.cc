#include "Accessor.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

Accessor::Accessor(std::unique_ptr<AccessorData> data, grib_section* p, grib_action* creator)
    : data_{std::move(data)}
    , name_{creator->name}
    , nameSpace_{creator->name_space}
    , context_{p->h->context}
    , handle_{p->h} // for now!
    , creator_{creator}
    , allNames_{{name_}}
    , allNameSpaces_{{nameSpace_}}
{
    // TODO
    // Finish "Gen" Init;
}

void Accessor::dump(grib_dumper const& dumper) const
{
    return data_->dump(dumper);
}

long Accessor::nextOffset() const
{
    return GRIB_NOT_IMPLEMENTED; // TO DO
}

std::size_t Accessor::stringLength() const
{
    return data_->stringLength();
}

int Accessor::valueCount(long& count) const
{
    return data_->valueCount(count);
}

long Accessor::byteCount() const
{
    return data_->byteCount();
}

long Accessor::byteOffset() const
{
    return data_->byteOffset();
}

int Accessor::nativeType() const
{
    return data_->nativeType();
}

grib_section* Accessor::subSection() const
{
    return subSection_;
}

int Accessor::notify_change(AccessorPtr const observed) const
{
    return GRIB_NOT_IMPLEMENTED; // TO DO
}

void Accessor::updateSize(std::size_t s) const
{
    return data_->updateSize(s);
}

std::size_t Accessor::preferredSize(int fromHandle) const
{
    return data_->preferredSize(fromHandle);
}

void Accessor::resize(size_t newSize) const
{
    return data_->resize(newSize);
}

int Accessor::nearestSmallerValue(double val, double& nearest) const
{
    return data_->nearestSmallerValue(val, nearest);
}

AccessorPtr Accessor::next(int mod)
{
    return nullptr; // TO DO
}

int Accessor::compare(AccessorPtr const rhs) const
{
    return data_->compare(*rhs->data_);
}

int Accessor::packMissing() const
{
    return data_->packMissing();
}

int Accessor::isMissing() const
{
    return data_->isMissing();
}

int Accessor::pack(grib_expression const& expression)
{
    return data_->pack(expression);
}

int Accessor::unpackSubarray(std::vector<double> &values, std::size_t start) const
{
    return data_->unpackSubarray(values, start);
}

}

