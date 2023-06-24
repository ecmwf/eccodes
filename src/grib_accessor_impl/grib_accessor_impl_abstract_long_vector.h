#ifndef _GRIB_ACCESSOR_IMPL_abstract_long_vector_H_20230621_
#define _GRIB_ACCESSOR_IMPL_abstract_long_vector_H_20230621_

#include "grib_accessor_impl_gen.h"

namespace eccodes {

    // Note: This isn't actually an abstract class as it doesn't have any pure virtual
    //       functions, but we retain the name for consistency with the C code

    class grib_accessor_impl_abstract_long_vector : public grib_accessor_impl_gen {
    protected:
        // grib_accessor_class_abstract_long_vector data
        // 
        // In order to support interop with the existing C code, this should be
        // kept in sync with grib_accessor_class_abstract_long_vector
        long* v{};
        long pack_index{};
        int number_of_elements{};

    public:
        grib_accessor_impl_abstract_long_vector(grib_section* p, grib_action* ga_creator);
        void init(const long len, grib_arguments* params) override;
    };
}

#endif // _GRIB_ACCESSOR_IMPL_abstract_long_vector_H_20230621_
