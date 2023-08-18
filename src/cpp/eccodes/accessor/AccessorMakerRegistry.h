#ifndef _ACCESSOR_REGISTRY_H_20230717_
#define _ACCESSOR_REGISTRY_H_20230717_

#include "AccessorDefs.h"

namespace eccodes::accessor{

class AccessorMaker;

void registerMaker(AccessorType const& type, AccessorMaker const* maker);
AccessorMaker const* getMaker(AccessorType const& type);

}

#endif // _ACCESSOR_REGISTRY_H_20230717_
