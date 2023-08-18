#include "AccessorMakerRegistry.h"
#include "AccessorMaker.h"

#include <mutex>
#include <map>

#include "grib_api_internal.h"

namespace eccodes::accessor {  

std::mutex makers_mutex;
auto& getMakers()
{
   static auto makers = [](){
      std::map<AccessorType, AccessorMaker const*> makers_;
      return makers_;
   }();

   return makers;
}

void registerMaker(AccessorType const& type, AccessorMaker const* maker)
{
   std::lock_guard<std::mutex> guard(makers_mutex);

   auto& makers = getMakers();
   if(auto it = makers.find(type); it != makers.end())
   {
      Assert(false);
   }

   makers[type] = maker;
}

AccessorMaker const* getMaker(AccessorType const& type)
{
   std::lock_guard<std::mutex> guard(makers_mutex);

   auto& makers = getMakers();

   if(auto it = makers.find(type); it != makers.end())
   {
      return it->second;
   }

   return nullptr;
}

}
