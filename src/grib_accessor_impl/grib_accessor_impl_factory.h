#ifndef _GRIB_ACCESSOR_IMPL_FACTORY_H_20230605_
#define _GRIB_ACCESSOR_IMPL_FACTORY_H_20230605_

#include "grib_accessor_impl_gen.h"

struct grib_action;

namespace eccodes {

    grib_accessor_impl_gen* create_grib_accessor_impl(grib_section* p, grib_action* creator);
 }

#endif // _GRIB_ACCESSOR_IMPL_FACTORY_H_20230605_
