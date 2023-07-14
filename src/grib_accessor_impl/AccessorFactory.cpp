#include "AccessorFactory.h"
#include "Accessor.h"
#include "Accessor_Utils.h"
#include "AccessorMaker.h"
#include "grib_api_internal.h"

#include "ProjString.h"

#include <vector>
#include <algorithm>

namespace eccodes {  

   static std::vector<Accessor> accessorStore{};

   AccessorPtr makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg)
   {
      if(cppMode() != CppMode::enabled) { return {}; }
      if(!creator->op)                  { return {}; }

      if(auto maker = getMaker(creator->op))
      {
         accessorStore.emplace_back(maker->pimpl->makeAccessor(p, creator, len, arg));
         return AccessorPtr(&accessorStore.back());
      }

      return {};
   }

   // Check if the grib_accessor* actually points to a valid class, and return
   // a pointer to the class if so, or nullptr
   AccessorPtr getAccessor(grib_accessor* a)
   {
      if(!a || a->cclass) { return {}; }

      if(auto it = std::find_if(std::begin(accessorStore), std::end(accessorStore),
                                [&a](Accessor const& accessor){ return a==asGribAccessor(accessor);}); 
                                it != accessorStore.end())
      {
         return AccessorPtr(&*it);
      }

      return {};
   }

    int destroyAccessor(grib_accessor* a)
    {
      if(auto it = std::find_if(std::begin(accessorStore), std::end(accessorStore),
                                [&a](Accessor const& accessor){ return a==asGribAccessor(accessor);}); 
                                it != accessorStore.end())
      {
         accessorStore.erase(it);
         return GRIB_SUCCESS;
      }

      return GRIB_NOT_FOUND;
    }

#if 0
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

#endif
}
