#ifndef _GRIB_ACCESSOR_IMPL_FACTORY_H_20230605_
#define _GRIB_ACCESSOR_IMPL_FACTORY_H_20230605_

#include "grib_accessor_impl_gen.h"

struct grib_action;

namespace eccodes {

    grib_accessor_impl_gen* create_grib_accessor_impl(grib_section* p, grib_action* creator);
    int destroy_grib_accessor_impl(grib_accessor* a);

    grib_accessor_impl_gen* get_grib_accessor_impl(grib_accessor* a);
 }

#endif // _GRIB_ACCESSOR_IMPL_FACTORY_H_20230605_
