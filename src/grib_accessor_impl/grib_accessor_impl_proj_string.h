#ifndef _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_
#define _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_

#include "grib_accessor_impl_gen.h"

namespace eccodes {
    class grib_accessor_impl_proj_string : public grib_accessor_impl_gen {
    public:
        // grib_accessor_TEMPLATE data
        // 
        // WARNING: Whilst converting from C to C++ the code supports casting
        //          this class to the equivalent grib_accessor_TEMPLATE C struct
        //
        //          This is only possible if the member variables MATCH EXACTLY!
        //
        //          DO NOT ADD any extra member variables to this class otherwise
        //          this functionality will break!
        //
        const char* grid_type;
        int endpoint;

    public:
        grib_accessor_impl_proj_string(grib_section* p, grib_action* ga_creator);
        void init(const long len, grib_arguments* params) override;
        int native_type() override { return GRIB_TYPE_STRING; }

    protected:
        int unpack_string(char_view chars) override;

    private:
        void init_proj_string(const long len, grib_arguments* params);
    };
}

#endif // _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_
