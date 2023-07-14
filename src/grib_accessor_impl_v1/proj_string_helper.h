#ifndef _PROJ_STRING_HELPER_H_20230620_
#define _PROJ_STRING_HELPER_H_20230620_

struct grib_handle;

namespace eccodes {

    // Function pointer than takes a handle and returns the proj string
    typedef int (*proj_func)(grib_handle*, char*);

    int get_proj_func(const char* grid_type, proj_func &func);
}

#endif // _PROJ_STRING_HELPER_H_20230620_
