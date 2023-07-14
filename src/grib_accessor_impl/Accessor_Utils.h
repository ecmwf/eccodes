#ifndef _GRIB_CPP_UTILS_H_20230624_
#define _GRIB_CPP_UTILS_H_20230624_

namespace eccodes {

   // Set env var grib_cpp to 1 to enable C++ mode:
   // $ export cpp_mode=1
   enum class CppMode { notSet, enabled, disabled};
   CppMode cppMode();

}

#endif // _GRIB_CPP_UTILS_H_20230624_
