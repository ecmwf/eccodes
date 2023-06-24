#ifndef _GA_IMPL_CREATORS_H_20230624_
#define _GA_IMPL_CREATORS_H_20230624_

#include <functional>
#include <string_view>

// Helper function to provide the factory with the correct "creator" for the requested ga_impl type

struct grib_section;
struct grib_action;

namespace eccodes {
    class grib_accessor_impl_gen;
    using ga_impl_creator = std::function<grib_accessor_impl_gen*(grib_section* p, grib_action* creator)>;

    ga_impl_creator get_ga_impl_creator(std::string_view name);
}

#endif // _GA_IMPL_CREATORS_H_20230624_
