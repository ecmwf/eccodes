#ifndef _ACCESSOR_STORE_H_20230817_
#define _ACCESSOR_STORE_H_20230817_

#include "AccessorDefs.h"
#include <memory>
#include <string>

namespace eccodes::accessor {

bool addAccessor(AccessorPtr accessor);
AccessorPtr getAccessor(AccessorName const& name);
bool destroyAccessor(AccessorName const& name);

}

#endif // _ACCESSOR_STORE_H_20230817_
