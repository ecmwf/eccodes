#include "AccessorMaker.h"

#include <mutex>
#include <unordered_map>

#include "grib_api_internal.h"

namespace eccodes {  

std::mutex makers_mutex;
static std::unordered_map<std::string, AccessorMaker const*> makers;

void registerMaker(std::string const& name, AccessorMaker const* maker)
{
   std::lock_guard<std::mutex> guard(makers_mutex);

   if(auto it = makers.find(name); it != makers.end())
   {
      Assert(false);
   }

   makers[name] = maker;
}

AccessorMaker const* getMaker(std::string const& name)
{
   std::lock_guard<std::mutex> guard(makers_mutex);

   if(auto it = makers.find(name); it != makers.end())
   {
      return it->second;
   }

   return nullptr;
}

}
