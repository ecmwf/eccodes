#pragma once

// C++ implementation of the existing grib_bits_fast_big_endian.cc

#include <string>
#include <memory>

namespace eccodes::accessor {

struct GribAction
{
    std::string name_;
    std::string op_;
    std::string nameSpace_;
    unsigned long flags_;
    //char* defaultkey;              /** name of the key used as default if not found */
    //grib_arguments* default_value; /** default expression as in .def file */
    //char* set;
    //char* debug_info; /** purely for debugging and tracing */
};

using GribActionPtr = std::shared_ptr<GribAction>;

}
