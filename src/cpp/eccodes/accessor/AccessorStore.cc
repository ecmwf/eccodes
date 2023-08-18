#include "AccessorStore.h"
#include "AccessorDefs.h"
#include "Accessor.h"
#include "AccessorMaker.h"

#include <vector>
#include <algorithm>
#include <mutex>

namespace {
   using AccessorEntry = std::pair<eccodes::accessor::AccessorName, eccodes::accessor::AccessorPtr>;
   static std::vector<AccessorEntry> accessorStore{};
   static std::size_t bufferOffset{0};
   std::mutex store_mutex;
}

namespace eccodes::accessor {  

bool addAccessor(AccessorPtr accessor)
{
   std::lock_guard<std::mutex> guard(store_mutex);

   // TODO: Handle duplicates ("same")

   accessorStore.push_back({accessor->name(), accessor});

   return true;
}

AccessorPtr getAccessor(AccessorName const& name)
{
   std::lock_guard<std::mutex> guard(store_mutex);

   if(auto it = std::find_if(std::begin(accessorStore), std::end(accessorStore),
                              [&name](AccessorEntry const& entry) { return name.get() == entry.first.get(); });
      it != accessorStore.end())
   {
      return it->second;
   }

   return {};
}

bool destroyAccessor(AccessorName const& name)
{
   std::lock_guard<std::mutex> guard(store_mutex);

   if(auto it = std::find_if(std::begin(accessorStore), std::end(accessorStore),
                              [&name](AccessorEntry const& entry) { return name.get() == entry.first.get(); });
      it != accessorStore.end())
   {
      accessorStore.erase(it);
      return true;
   }

   return false;
}

// To implement in C++
// struct grib_context;
// struct grib_handle;
// struct grib_action;
// struct grib_section;
// struct grib_dumper;
// struct grib_expression;
// struct grib_arguments;
// long nextOffset() const;
// long byteCount() const;
// long byteOffset() const;
// grib_section* subSection() const;
// int notify_change(AccessorPtr const observed) const;
// void updateSize(std::size_t s) const;
// std::size_t preferredSize(int fromHandle) const;
// void resize(size_t newSize) const;
// AccessorPtr next(int mod);
// grib_context* context_{};   // Conversion helper - will change later...
// grib_handle* handle_{};     // Conversion helper - will change later...
// grib_action* creator_{};    // Conversion helper - will change later...
// std::weak_ptr<Accessor> parent_{};
// std::weak_ptr<Accessor> next_{};
// std::weak_ptr<Accessor> previous_{};
// grib_section* subSection_{}; // Conversion helper - will change later...


}
