#include "grib_accessor_impl_factory.h"
#include "ga_impl_creators.h"
#include "grib_cpp_utils.h"

#include <unordered_set>

namespace eccodes {  

   std::unordered_set<grib_accessor_impl_gen*> ga_store{};

   grib_accessor_impl_gen* create_grib_accessor_impl(grib_section* p, grib_action* creator)
   {
      if(get_cpp_mode() != cpp_mode::enabled) { return nullptr; }
      if(!creator->op)                        { return nullptr; }

      grib_accessor_impl_gen* ga_impl{};

      if(auto impl_creator = get_ga_impl_creator(creator->op)) 
      {
         ga_impl = impl_creator(p, creator);
         if(ga_impl) { ga_store.insert(ga_impl); }
      }

      return ga_impl;
   }

   int destroy_grib_accessor_impl(grib_accessor* a)
   {
      grib_accessor_impl_gen* ga_impl = (grib_accessor_impl_gen*)a;

      if(ga_store.count(ga_impl) > 0) {
         ga_store.erase(ga_impl);
         delete ga_impl;
         return GRIB_SUCCESS;
      }

      return GRIB_NOT_FOUND;
   }

   // Check if the grib_accessor* actually points to a valid class, and returns
   // a pointer to the class if so, or nullptr
   grib_accessor_impl_gen* get_grib_accessor_impl(grib_accessor* a)
   {
      if(!a || a->cclass) { return nullptr; }

      if(ga_store.count((grib_accessor_impl_gen*)a) > 0) {
         return (grib_accessor_impl_gen*)a;
      }

      return nullptr;
   }
 }
