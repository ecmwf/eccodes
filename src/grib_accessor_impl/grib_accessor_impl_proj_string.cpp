#include "grib_accessor_impl_proj_string.h"

namespace eccodes {  
    grib_accessor_impl_proj_string::grib_accessor_impl_proj_string(grib_section* p, grib_action* creator) :
        grib_accessor_impl_gen(p, creator)
    {
        // No extra logic here - init() handles this
    }

    void grib_accessor_impl_proj_string::init(const long len, grib_arguments* params)
    {
        init_gen(len, params);
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

    grib_section* grib_accessor_impl_proj_string::sub_section()
    {
        return nullptr; // TO DO
    }

    int grib_accessor_impl_proj_string::pack_missing()
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::is_missing()
    {
        return -1; // TO DO
    }

    int grib_accessor_impl_proj_string::pack_string_array(const char** /* v */, size_t* /* len */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::pack_expression(grib_expression* /* e */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_bytes(unsigned char* /* val */, size_t* /* len */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_double(double* /* val */, size_t* /* len */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_float(float* /* val */, size_t* /* len */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_long(long* /* val */, size_t* /* len */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_string(char* /* val */, size_t* /* len */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_string_array(char** /* v */, size_t* /* len */)
    {
        return -1; // TO DO
    }
    
    size_t grib_accessor_impl_proj_string::string_length()
    {
        return -1; // TO DO
    }
    
    long grib_accessor_impl_proj_string::byte_count()
    {
        return -1; // TO DO
    }
    
    long grib_accessor_impl_proj_string::byte_offset()
    {
        return -1; // TO DO
    }
    
    long grib_accessor_impl_proj_string::next_offset()
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::value_count(long* /* count */)
    {
        return -1; // TO DO
    }
    
    void grib_accessor_impl_proj_string::dump(grib_dumper* /* dumper */)
    {
    }
    
    void grib_accessor_impl_proj_string::post_init()
    {
    }
    
    int grib_accessor_impl_proj_string::notify_change()
    {
        return -1; // TO DO
    }
    
    void grib_accessor_impl_proj_string::update_size(size_t /* s */)
    {
    }
    
    size_t grib_accessor_impl_proj_string::preferred_size(int /* from_handle */)
    {
        return -1; // TO DO
    }
    
    void grib_accessor_impl_proj_string::resize(size_t /* new_size */)
    {
    }
    
    int grib_accessor_impl_proj_string::nearest_smaller_value (double /* val */, double* /* nearest */)
    {
        return -1; // TO DO
    }
    
    grib_accessor_impl* grib_accessor_impl_proj_string::next_accessor(int /* mod */)
    {
        return nullptr; // TO DO
    }
    
    int grib_accessor_impl_proj_string::compare()
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_double_element(size_t /* i */, double* /* val */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_float_element(size_t /* i */, float* /* val */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_double_element_set(const size_t* /* index_array */, size_t /* len */, double* /* val_array */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_float_element_set(const size_t* /* index_array */, size_t /* len */, float* /* val_array */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::unpack_double_subarray(double* /* val */, size_t /* start */, size_t /* len */)
    {
        return -1; // TO DO
    }
    
    int grib_accessor_impl_proj_string::clear()
    {
        return -1; // TO DO
    }
    
    grib_accessor_impl* grib_accessor_impl_proj_string::make_clone(grib_section* /* s */, int* /* err */)
    {
        return nullptr; // TO DO
    }
    
}
