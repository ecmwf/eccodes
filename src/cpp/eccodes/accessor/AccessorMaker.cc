#include "AccessorMaker.h"

#include <mutex>
#include <map>

#include "grib_api_internal.h"

namespace eccodes::accessor {  

std::mutex makers_mutex;
auto getMakers()
{
   static auto makers = [](){
      std::map<std::string, eccodes::accessor::AccessorMaker const*> makers_;
      return makers_;
   }();

   return makers;
}

void registerMaker(std::string const& name, AccessorMaker const* maker)
{
   std::lock_guard<std::mutex> guard(makers_mutex);

   auto makers = getMakers();
   if(auto it = makers.find(name); it != makers.end())
   {
      Assert(false);
   }

   makers[name] = maker;
}

AccessorMaker const* getMaker(std::string const& name)
{
   std::lock_guard<std::mutex> guard(makers_mutex);

   auto makers = getMakers();

   if(auto it = makers.find(name); it != makers.end())
   {
      return it->second;
   }

   return nullptr;
}

}
