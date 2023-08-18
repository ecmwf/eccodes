#include "Accessor.h"
#include "AccessorMakerRegistry.h"
#include "AccessorMaker.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

AccessorPtr makeAccessor(AccessorType const& type, AccessorName const& name, AccessorNameSpace const& nameSpace, AccessorInitData const& initData)
{
   if(type.get().empty()) { return {}; }

   if(auto maker = getMaker(type))
   {
      return maker->pimpl->makeAccessorImpl(name, nameSpace, initData);
   }

   return {};
}

Accessor::Accessor(AccessorName const& name, AccessorNameSpace const& nameSpace, std::unique_ptr<AccessorData> data)
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

int Accessor::nativeType() const
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

bool Accessor::pack(grib_expression const& expression)
{
    return data_->pack(expression);
}

bool Accessor::packMissing() const
{
    return data_->packMissing();
}

std::vector<double> Accessor::unpackSubarray(std::size_t start) const
{
    std::vector<double> values{};
    return data_->unpackSubarray(values, start) ? values : std::vector<double>{};
}

}

