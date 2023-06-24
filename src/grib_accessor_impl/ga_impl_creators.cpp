#include "ga_impl_creators.h"

#include <unordered_map>

// Creator targets
#include "grib_accessor_impl_proj_string.h"

namespace eccodes {  

   using namespace std::string_view_literals;

   template<typename GA_TYPE>
   std::enable_if_t<std::is_base_of_v<grib_accessor_impl_gen, GA_TYPE>, grib_accessor_impl_gen*>
   create_ga_impl(grib_section* p, grib_action* creator)
   {
      return new GA_TYPE(p, creator);
   }

   std::unordered_map<std::string_view, ga_impl_creator> ga_impl_creators = {
      {"proj_string"sv, create_ga_impl<grib_accessor_impl_proj_string>}
   };

   ga_impl_creator get_ga_impl_creator(std::string_view name)
   {
      if(auto it = ga_impl_creators.find(name); it != ga_impl_creators.end()) 
      {
         return it->second;
      }

      return {};
   }

}
