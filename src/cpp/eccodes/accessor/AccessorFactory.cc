#include "AccessorFactory.h"
#include "Accessor.h"
#include "AccessorMaker.h"
#include "grib_api_internal.h"

#include <vector>
#include <algorithm>
#include <unordered_map>

namespace {
   static std::unordered_map<std::string,eccodes::accessor::AccessorPtr> accessorStore{};
}

namespace eccodes::accessor {  

   AccessorPtr makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg)
   {
      std::string name{creator->op};

      if(name.empty()) { return {}; }

      if(auto maker = getMaker(name))
      {
         accessorStore.emplace(name, maker->pimpl->makeAccessorImpl(p, creator, len, arg));
         return accessorStore[name];
      }

      return {};
   }

   // Check if the grib_accessor* actually points to a valid class, and return
   // a pointer to the class if so, or nullptr
   AccessorPtr getAccessor(std::string const& name)
   {
      if(auto it = accessorStore.find(name); it != accessorStore.end())
      {
         return it->second;
      }

      return {};
   }

    int destroyAccessor(std::string const& name)
    {
      if(auto it = accessorStore.find(name); it != accessorStore.end())
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
