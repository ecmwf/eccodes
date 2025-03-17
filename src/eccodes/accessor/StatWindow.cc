/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "StatWindow.h"
#include "step.h"
#include <sstream>

eccodes::accessor::StatWindow _grib_accessor_stat_window{};
grib_accessor* grib_accessor_stat_window = &_grib_accessor_stat_window;


namespace eccodes::accessor
{

void StatWindow::init(const long l, grib_arguments* args)
{
  Gen::init(l, args);
  int n = 0;
  time_range_key_ = args->get_name(grib_handle_of_accessor(this), n++);
  unit_range_key_ = args->get_name(grib_handle_of_accessor(this), n++);
}

int StatWindow::unpack_string(char *val, size_t *len)
{
  int ret = 0;
  long time_range_value = 0;
  if ((ret = grib_get_long(grib_handle_of_accessor(this), time_range_key_, &time_range_value)) != GRIB_SUCCESS)
    return ret;
  long unit_range_value = 0;
  if ((ret = grib_get_long(grib_handle_of_accessor(this), unit_range_key_, &unit_range_value)) != GRIB_SUCCESS)
    return ret;

  Step step{time_range_value, unit_range_value};
  step.optimize_unit();

  std::stringstream ss;
  ss << step.value<std::string>("%g", true);

  *len = ss.str().size();
  memcpy(val, ss.str().c_str(), *len);

  return GRIB_SUCCESS;
}

long StatWindow::get_native_type()
{
    return GRIB_TYPE_STRING;
}

}  // namespace eccodes::accessor

