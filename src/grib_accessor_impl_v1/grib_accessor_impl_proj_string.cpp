#include "grib_accessor_impl_proj_string.h"
#include "proj_string_helper.h"

namespace eccodes {  
    grib_accessor_impl_proj_string::grib_accessor_impl_proj_string(grib_section* p, grib_action* ga_creator) :
        grib_accessor_impl_gen(p, ga_creator)
    {
        // No extra logic here - init() handles this
    }

    void grib_accessor_impl_proj_string::init(const long len, grib_arguments* params)
    {
        // Default is to call parent's init, then init self (i.e. like a constructor)
        grib_accessor_impl_gen::init(len, params);
        init_proj_string(len, params);
    }

    void grib_accessor_impl_proj_string::init_proj_string(const long /* len */, grib_arguments* params)
    {
        this->grid_type = grib_arguments_get_name(accessor_handle(), params, 0);
        this->endpoint  = grib_arguments_get_long(accessor_handle(), params, 1);
        this->length    = 0;
        this->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
        this->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    }
    
    #define ENDPOINT_SOURCE 0
    #define ENDPOINT_TARGET 1

    int grib_accessor_impl_proj_string::unpack_string(char_view chars)
    {
        int err = 0, found = 0;
        size_t i           = 0;
        char target_grid_type[64] = {0,};
        grib_handle* h = accessor_handle();
        size_t size    = sizeof(target_grid_type) / sizeof(*target_grid_type);

        Assert(endpoint == ENDPOINT_SOURCE || endpoint == ENDPOINT_TARGET);

        err = grib_get_string(h, grid_type, target_grid_type, &size);
        if (err) return err;

        proj_func func{};
        err = get_proj_func(target_grid_type, func);
        if(err == GRIB_SUCCESS)
        {
            if (endpoint == ENDPOINT_SOURCE) {
                snprintf(chars.ptr, 64, "EPSG:4326");
            }
            else {
                // Invoke the appropriate function to get the target proj string
                if ((err = func(h, chars.ptr)) != GRIB_SUCCESS) return err;
            }
        }
        else if(err == GRIB_NOT_FOUND)
        {
            *chars.len = 0;
            return GRIB_NOT_FOUND;
        }
        else return err;

        size = strlen(chars.ptr);
        Assert(size > 0);
        *chars.len = size + 1;
        return err;
    }
    
}
