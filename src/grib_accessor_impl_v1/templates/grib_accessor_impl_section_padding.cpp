#include "grib_accessor_impl_TEMPLATE.h"

namespace eccodes {

    grib_accessor_impl_TEMPLATE::grib_accessor_impl_TEMPLATE(grib_section* p, grib_action* ga_creator) :
        grib_accessor_impl_PARENT(p, ga_creator)
    {
        // No extra logic here - init() handles this
    }

    grib_accessor_impl_TEMPLATE::~grib_accessor_impl_TEMPLATE() 
    {}

    void grib_accessor_impl_TEMPLATE::init(const long len, grib_arguments* params)
    {
        // Default is to call parent's init, then init self (i.e. like a constructor)
        grib_accessor_impl_PARENT::init(len, params);
        init_TEMPLATE(len, params);
    }

    void grib_accessor_impl_TEMPLATE::init_TEMPLATE(const long len, grib_arguments* params) {}

    int grib_accessor_impl_TEMPLATE::pack_missing() { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::is_missing() { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::pack_string_array(const char** v, size_t* len) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::pack_expression(grib_expression* e) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_string_array(char** v, size_t* len) { return GRIB_NOT_IMPLEMENTED; }
    size_t grib_accessor_impl_TEMPLATE::string_length() { return GRIB_NOT_IMPLEMENTED; }
    long grib_accessor_impl_TEMPLATE::byte_count() { return GRIB_NOT_IMPLEMENTED; }
    long grib_accessor_impl_TEMPLATE::byte_offset() { return GRIB_NOT_IMPLEMENTED; }
    long grib_accessor_impl_TEMPLATE::next_offset() { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::value_count(long* count) { return GRIB_NOT_IMPLEMENTED; }
    void grib_accessor_impl_TEMPLATE::dump(grib_dumper* dumper) {}
    void grib_accessor_impl_TEMPLATE::post_init() {}
    int grib_accessor_impl_TEMPLATE::notify_change(grib_accessor* observed) { return GRIB_NOT_IMPLEMENTED; }
    void grib_accessor_impl_TEMPLATE::update_size(size_t s) {}
    size_t grib_accessor_impl_TEMPLATE::preferred_size(int from_handle) { return GRIB_NOT_IMPLEMENTED; }
    void grib_accessor_impl_TEMPLATE::resize(size_t new_size) {}
    int grib_accessor_impl_TEMPLATE::nearest_smaller_value (double val, double* nearest) { return GRIB_NOT_IMPLEMENTED; }
    grib_accessor* grib_accessor_impl_TEMPLATE::next_accessor(int mod) { return NULL; }
    int grib_accessor_impl_TEMPLATE::compare(grib_accessor_impl* ga_impl) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_double_element(size_t i, double* val) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_float_element(size_t i, float* val) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_float_element_set(const size_t* index_array, size_t len, float* val_array) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_double_subarray(double* val, size_t start, size_t len) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::clear() { return GRIB_NOT_IMPLEMENTED; }
    grib_accessor_impl* grib_accessor_impl_TEMPLATE::make_clone(grib_section* s, int* err) { return NULL; }

    int grib_accessor_impl_TEMPLATE::pack_bytes(pack_buffer& bytes, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::pack_double(pack_buffer& doubles, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::pack_float(pack_buffer& floats, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::pack_long(pack_buffer& longs, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::pack_string(pack_buffer& chars, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }

    int grib_accessor_impl_TEMPLATE::unpack_bytes(byte_view bytes) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_double(double_view doubles) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_float(float_view floats) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_long(long_view longs) { return GRIB_NOT_IMPLEMENTED; }
    int grib_accessor_impl_TEMPLATE::unpack_string(char_view chars) { return GRIB_NOT_IMPLEMENTED; }

}
