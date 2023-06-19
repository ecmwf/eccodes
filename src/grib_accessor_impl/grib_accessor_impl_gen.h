#ifndef _GRIB_ACCESSOR_IMPL_GEN_H_20230519_
#define _GRIB_ACCESSOR_IMPL_GEN_H_20230519_

#include "grib_accessor_impl.h"
#include "grib_api_internal.h"
#include "type_view.h"
#include "pack_buffer.h"
#include <vector>

namespace eccodes {

    // In order to support interop with the existing C code, we inherit from
    // grib_accessor as well as the interface
    //
    // This allows C code to safely cast a pointer to any class from this point
    // in the hierarchy (so NOT the base grib_accessor_impl) to grib_accessor*
    //
    // This will be removed once all grib_accessors have been migrated
    //
    class grib_accessor_impl_gen : public grib_accessor_impl, public grib_accessor {
    private:
        int native_type_def{GRIB_TYPE_UNDEFINED};
        size_t str_len{1024};

    public:
        grib_accessor_impl_gen(grib_section* p, grib_action* creator);
        void init(const long len, grib_arguments* params) override;
        grib_section* sub_section() override { return nullptr; }
        int native_type() override { return native_type_def; }
        int pack_missing() override { return GRIB_INVALID_TYPE; }
        int is_missing() override;
        int pack_bytes(const unsigned char* val, size_t* len) final;
        int pack_double(const double* val, size_t* len) final;
        int pack_float(const float* val, size_t* len) final;
        int pack_long(const long* val, size_t* len) final;
        int pack_string(const char* val, size_t* len) final;
        int pack_string_array(const char** v, size_t* len) override;
        int pack_expression(grib_expression* e) override;
        int unpack_bytes(unsigned char* val, size_t* len) override;
        int unpack_double(double* val, size_t* len) override;
        int unpack_float(float* val, size_t* len) override;
        int unpack_long(long* val, size_t* len) override;
        int unpack_string(char* val, size_t* len) override;
        int unpack_string_array(char** v, size_t* len) override;
        size_t string_length() override { return str_len; }
        long byte_count() override { return length; }
        long byte_offset() override { return offset; }
        long next_offset() override { return offset + length; }
        int value_count(long* count) override;
        void dump(grib_dumper* dumper) override;
        void post_init() override;
        int notify_change() override;
        void update_size(size_t s) override;
        size_t preferred_size(int from_handle) override;
        void resize(size_t new_size) override;
        int nearest_smaller_value (double val, double* nearest) override;
        grib_accessor_impl* next_accessor(int mod) override;
        int compare() override;
        int unpack_double_element(size_t i, double* val) override;
        int unpack_float_element(size_t i, float* val) override;
        int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;
        int unpack_float_element_set(const size_t* index_array, size_t len, float* val_array) override;
        int unpack_double_subarray(double* val, size_t start, size_t len) override;
        int clear() override;
        grib_accessor_impl* make_clone(grib_section* s, int* err) override;

    protected:
        virtual void init_gen(const long len, grib_arguments* params);

        virtual int pack_bytes(pack_buffer& bytes, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
        virtual int pack_double(pack_buffer& doubles, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
        virtual int pack_float(pack_buffer& floats, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
        virtual int pack_long(pack_buffer& longs, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }
        virtual int pack_string(pack_buffer& chars, std::size_t* packed_len) { return GRIB_NOT_IMPLEMENTED; }

        template<typename T>
        int unpack(T* val, size_t* len);

        virtual int unpack_bytes(byte_view bytes) { return GRIB_NOT_IMPLEMENTED; }
        virtual int unpack_double(double_view doubles) { return GRIB_NOT_IMPLEMENTED; }
        virtual int unpack_float(float_view floats) { return GRIB_NOT_IMPLEMENTED; }
        virtual int unpack_long(long_view longs) { return GRIB_NOT_IMPLEMENTED; }
        virtual int unpack_string(char_view chars) { return GRIB_NOT_IMPLEMENTED; }

        // Helpers
        grib_accessor* as_accessor() { return static_cast<grib_accessor*>(this); }
        grib_handle* accessor_handle();
    };
}

#endif // _GRIB_ACCESSOR_IMPL_GEN_H_20230519_
