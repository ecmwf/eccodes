#include "AccessorProxy.h"
#include "Accessor.h"
#include "AccessorStore.h"
#include "AccessorUtils/AccessorException.h"
#include "AccessorUtils/TypeString.h"

namespace eccodes::accessor {

template<typename T>
T get(AccessorName const& name)
{
    if(auto accessor = AccessorStore::instance().getAccessor(name); accessor)
    {
        if(auto values = accessor->unpack<T>(); !values.empty())
        {
            return values[0];
        }
    }

    throw AccessorException("AccessorProxy - couldn't get " + typeString<T>() + " for accessor [" + name.get() + ']');
    return T{};
}

double getDouble(AccessorName const& name)
{
    return get<double>(name);
}

long getLong(AccessorName const& name)
{
    return get<long>(name);
}

}
