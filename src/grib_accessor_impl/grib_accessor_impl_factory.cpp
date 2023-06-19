#include "grib_accessor_impl_factory.h"

#include "grib_accessor_impl_proj_string.h"

namespace eccodes {  

     grib_accessor_impl_gen* create_grib_accessor_impl(grib_section* p, grib_action* creator)
     {
        if(!creator->op) { return nullptr; }

        if(strcmp(creator->op,"proj_string") == 0) { return new grib_accessor_impl_proj_string(p, creator); }

        return nullptr;
     }

 }
