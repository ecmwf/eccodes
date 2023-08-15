#ifndef _ACCESSOR_FACTORY_H_20230706_
#define _ACCESSOR_FACTORY_H_20230706_

#include <memory>
#include <string>

struct grib_section;
struct grib_action;
struct grib_arguments;
struct grib_accessor;

namespace eccodes::accessor {

class Accessor;
using AccessorPtr = std::shared_ptr<Accessor>;

AccessorPtr makeAccessor(grib_section* p, grib_action* creator, const long len, grib_arguments* arg);
AccessorPtr getAccessor(std::string const& name);
int destroyAccessor(std::string const& name);

}

#endif // _ACCESSOR_FACTORY_H_20230706_
