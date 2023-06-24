#ifndef _GRIB_ACCESSOR_IMPL_g1step_range_H_2023MMDD_
#define _GRIB_ACCESSOR_IMPL_g1step_range_H_2023MMDD_

#include "grib_accessor_impl_abstract_long_vector.h"

namespace eccodes {

    class grib_accessor_impl_g1step_range : public grib_accessor_impl_abstract_long_vector {
    protected:
        // grib_accessor_class_g1step_range data
        // 
        // In order to support interop with the existing C code, this should be
        // kept in sync with grib_accessor_class_g1step_range
        const char* p1{};
        const char* p2{};
        const char* timeRangeIndicator{};
        const char *unit{};
        const char *step_unit{};
        const char *stepType{};
        const char *patch_fp_precip{};
        int error_on_units{};

    public:
        grib_accessor_impl_g1step_range(grib_section* p, grib_action* ga_creator);
        ~grib_accessor_impl_g1step_range();
        void init(const long len, grib_arguments* params) override;
        int native_type() override { return GRIB_TYPE_STRING; }
        size_t string_length() override;
        int value_count(long* count) override;
        void dump(grib_dumper* dumper) override;

    protected:
        int pack_long(pack_buffer& longs, std::size_t* packed_len) override;
        int pack_string(pack_buffer& chars, std::size_t* packed_len) override;

        int unpack_long(long_view longs) override;
        int unpack_string(char_view chars) override;

    private:
        void init_g1step_range(const long len, grib_arguments* params);
        int grib_g1_step_get_steps(long* start, long* theEnd);
    };
}

#endif // _GRIB_ACCESSOR_IMPL_g1step_range_H_2023MMDD_
