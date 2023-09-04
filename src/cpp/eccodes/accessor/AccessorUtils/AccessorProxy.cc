#include "AccessorProxy.h"
#include "Accessor.h"
#include "AccessorStore.h"
#include "AccessorUtils/AccessorException.h"

namespace eccodes::accessor {

namespace {
    auto getAccessor = [](AccessorName const& name) -> AccessorPtr {
        if(auto accessorPtr = AccessorStore::instance().getAccessor(name); accessorPtr)
        {
            return accessorPtr;
        }

        throw AccessorException("AccessorProxy - couldn't get accessor [" + name.get() + ']');
        return nullptr;
    };
}

double unpackDouble(AccessorName const& name)
{
    return getAccessor(name)->unpack<double>();
}

long unpackLong(AccessorName const& name)
{
    return getAccessor(name)->unpack<long>();
}

}
