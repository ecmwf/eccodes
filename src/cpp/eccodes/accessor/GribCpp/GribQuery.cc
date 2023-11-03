#include "GribQuery.h"
#include "Accessor.h"
#include "AccessorStore.h"
#include "AccessorUtils/AccessorException.h"

namespace eccodes::accessor {

AccessorPtr gribFindAccessor(AccessorName const& name)
{
        if(auto accessorPtr = AccessorStore::instance().getAccessor(name); accessorPtr)
        {
            return accessorPtr;
        }

        //throw AccessorException("AccessorProxy - couldn't get accessor [" + name.get() + ']');
        return nullptr;
}

}
