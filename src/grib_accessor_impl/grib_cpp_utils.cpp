#include "grib_cpp_utils.h"

#include <cstdlib>
#include <string_view>

namespace eccodes {
 
    using namespace std::string_view_literals;

    constexpr std::string_view grib_cpp_env = "grib_cpp"sv;
    constexpr std::string_view grib_cpp_env_enabled = "1"sv;

    cpp_mode get_cpp_mode()
    {
        static cpp_mode grib_cpp_mode = [](){
                if(const char* env_str = std::getenv(grib_cpp_env.data()); 
                   env_str && env_str == grib_cpp_env_enabled) {
                    //fprintf(stdout, "ECCODES DEBUG Using GRIB C++ classes\n");
                    return cpp_mode::enabled;
                }
                return cpp_mode::disabled;
            }();

        return grib_cpp_mode;
    }
}

