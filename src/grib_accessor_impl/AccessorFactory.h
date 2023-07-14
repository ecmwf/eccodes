#ifndef _ACCESSOR_FACTORY_H_20230706_
#define _ACCESSOR_FACTORY_H_20230706_

#include "AccessorPtr.h"

#include <memory>
#include <string_view>

struct grib_section;
struct grib_action;
struct grib_arguments;
struct grib_accessor;

namespace eccodes {
AccessorPtr makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg);
AccessorPtr getAccessor(grib_accessor* a);
int destroyAccessor(grib_accessor* a);

}

#endif // _ACCESSOR_FACTORY_H_20230706_
