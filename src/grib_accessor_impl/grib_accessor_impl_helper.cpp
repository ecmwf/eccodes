#include "grib_accessor_impl_helper.h"
#include "grib_api_internal.h"

namespace eccodes {  

   int compare_accessors(grib_accessor_impl_gen* ga_impl1, grib_accessor_impl_gen* ga_impl2, int compare_flags)
   {
      Assert(ga_impl1 && ga_impl2);

      // TODO: C version uses a custom strcmp - may need to check perf here...
      if ((compare_flags & GRIB_COMPARE_NAMES) && strcmp(ga_impl1->name, ga_impl2->name) != 0) {
         return GRIB_NAME_MISMATCH;
      }

      int type_mismatch{};
      if (compare_flags & GRIB_COMPARE_TYPES) {
         type_mismatch = ga_impl1->native_type() != ga_impl2->native_type() ? 1 : 0;
      }

      int ret = ga_impl1->compare(ga_impl2); 

      if (ret == GRIB_VALUE_MISMATCH && type_mismatch) { ret = GRIB_TYPE_AND_VALUE_MISMATCH; }

      return ret;
   }

}
