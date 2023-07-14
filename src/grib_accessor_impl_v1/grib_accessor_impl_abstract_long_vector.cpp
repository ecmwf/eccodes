#include "grib_accessor_impl_abstract_long_vector.h"

namespace eccodes {

    grib_accessor_impl_abstract_long_vector::grib_accessor_impl_abstract_long_vector(grib_section* p, grib_action* ga_creator) :
        grib_accessor_impl_gen(p, ga_creator)
    {
        // No extra logic here - init() handles this
    }

    void grib_accessor_impl_abstract_long_vector::init(const long len, grib_arguments* params)
    {
        grib_accessor_impl_gen::init(len, params);
    }

}
