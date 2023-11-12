#pragma once

// C++ implementation of the functions in grib_accessor_class.cc that are not directly part of the C++ classes

#include "AccessorDefs.h"
#include "AccessorData/AccessorInitData.h"
#include "GribCpp/GribStatus.h"
#include "GribStub/GribActionStub.h"

namespace eccodes::accessor {

AccessorPtr parentOf(AccessorPtr ptr);

GribStatus gribGetBlockLength(AccessorPtr ptr, size_t& l);

AccessorPtr gribAccessorFactory(GribActionPtr creator, const long len, AccessorInitData initData);
void gribPushAccessor(AccessorPtr ptr, std::vector<AccessorPtr> ptrList);
GribStatus gribSectionAdjustSizes(int update, int depth);

}
