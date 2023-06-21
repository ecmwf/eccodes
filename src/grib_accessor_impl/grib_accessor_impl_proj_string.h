#ifndef _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_
#define _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_

#include "grib_accessor_impl_gen.h"

namespace eccodes {
    class grib_accessor_impl_proj_string : public grib_accessor_impl_gen {
    public:
        // grib_accessor_proj_string data
        // 
        // In order to support interop with the existing C code, this should be
        // kept in sync with grib_accessor_class_proj_string
        const char* grid_type;
        int endpoint;

    public:
        grib_accessor_impl_proj_string(grib_section* p, grib_action* ga_creator);
        void init(const long len, grib_arguments* params) override;
        int native_type() override { return GRIB_TYPE_STRING; }
        int unpack_string(char* val, size_t* len) override;

    protected:
        virtual void init_proj_string(const long len, grib_arguments* params);
    };
}

#endif // _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_
