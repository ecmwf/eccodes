#include "Accessor_Utils.h"

#include <cstdlib>
#include <string_view>

namespace eccodes {
 
    using namespace std::string_view_literals;

    constexpr std::string_view cpp_mode_env = "cpp_mode"sv;
    constexpr std::string_view cpp_mode_env_enabled = "1"sv;

    CppMode cppMode()
    {
        static CppMode cppMode_ = [](){
                if(const char* envStr = std::getenv(cpp_mode_env.data()); 
                    envStr && envStr == cpp_mode_env_enabled) {
                    return CppMode::enabled;
                }
                return CppMode::disabled;
            }();

        return cppMode_;
    }
}

