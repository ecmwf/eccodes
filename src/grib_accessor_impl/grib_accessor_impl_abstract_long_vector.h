#ifndef _GRIB_ACCESSOR_IMPL_abstract_long_vector_H_20230621_
#define _GRIB_ACCESSOR_IMPL_abstract_long_vector_H_20230621_

#include "grib_accessor_impl_gen.h"

namespace eccodes {

    // Note: This isn't actually an abstract class as it doesn't have any pure virtual
    //       functions, but we retain the name for consistency with the C code

    class grib_accessor_impl_abstract_long_vector : public grib_accessor_impl_gen {
    protected:
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
        long* v{};
        long pack_index{};
        int number_of_elements{};

    public:
        grib_accessor_impl_abstract_long_vector(grib_section* p, grib_action* ga_creator);
        void init(const long len, grib_arguments* params) override;
    };
}

#endif // _GRIB_ACCESSOR_IMPL_abstract_long_vector_H_20230621_
