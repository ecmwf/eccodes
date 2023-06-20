#include "grib_accessor_impl_factory.h"

#include "grib_accessor_impl_proj_string.h"

#include <unordered_set>
#include <type_traits>
#include <cstdlib>

namespace eccodes {  

   std::unordered_set<grib_accessor_impl_gen*> ga_store{};

   // Set env var grib_cpp to 1 to enable C++ mode:
   // $ export grib_cpp=1
   const char grib_cpp_env[] = "grib_cpp";
   enum class cpp_mode { not_set, enabled, disabled};
   static cpp_mode get_cpp_mode()
   {
      static cpp_mode grib_cpp_mode = [](){
            if(const char* env_str = std::getenv(grib_cpp_env)){
               if(strcmp(env_str, "1") == 0) {
                  fprintf(stderr, "ECCODES DEBUG Using GRIB C++ classes\n");
                  return cpp_mode::enabled;
               }
            }
            return cpp_mode::disabled;
         }();

      return grib_cpp_mode;
   }

   // Helper to create the requested accessor and store a copy of it
   template<typename GA_TYPE>
   std::enable_if_t<std::is_base_of_v<grib_accessor_impl_gen, GA_TYPE>, grib_accessor_impl_gen*>
   create_and_store(grib_section* p, grib_action* creator)
   {
      grib_accessor_impl_gen* ga_impl = new GA_TYPE(p, creator);
      if(ga_impl) { ga_store.insert(ga_impl); }

      return ga_impl;
   }

   grib_accessor_impl_gen* create_grib_accessor_impl(grib_section* p, grib_action* creator)
   {
      if(get_cpp_mode() != cpp_mode::enabled) { return nullptr; }
      if(!creator->op)                        { return nullptr; }

      if(strcmp(creator->op,"proj_string") == 0) { return create_and_store<grib_accessor_impl_proj_string>(p, creator); }

      return nullptr;
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
