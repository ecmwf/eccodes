#ifndef _GRIB_CPP_UTILS_H_20230624_
#define _GRIB_CPP_UTILS_H_20230624_

namespace eccodes {

   // Set env var grib_cpp to 1 to enable C++ mode:
   // $ export grib_cpp=1
   enum class cpp_mode { not_set, enabled, disabled};
   cpp_mode get_cpp_mode();

}

#endif // _GRIB_CPP_UTILS_H_20230624_
