/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "TimeSpan.h"
#include "step.h"
#include <sstream>
#include <iostream>

eccodes::accessor::TimeSpan _grib_accessor_time_span{};
grib_accessor* grib_accessor_time_span = &_grib_accessor_time_span;


namespace eccodes::accessor
{

void TimeSpan::init(const long l, grib_arguments* args)
{
  Gen::init(l, args);
  int n = 0;
  grib_handle* hand = get_enclosing_handle();

  time_range_key_ = args->get_name(hand, n++);
  unit_range_key_ = args->get_name(hand, n++);
}

int TimeSpan::unpack_string(char *val, size_t *len)
{
  int ret = 0;
  long time_range_value = 0;
  grib_handle* hand = get_enclosing_handle();

  if ((ret = grib_get_long(hand, time_range_key_, &time_range_value)) != GRIB_SUCCESS)
    return ret;
  long unit_range_value = 0;
  if ((ret = grib_get_long(hand, unit_range_key_, &unit_range_value)) != GRIB_SUCCESS)
    return ret;

  Step step{time_range_value, unit_range_value};
  step.optimize_unit();

  std::stringstream ss;
  ss << step.value<std::string>("%g", true);

  *len = ss.str().size() + 1;
  memcpy(val, ss.str().c_str(), ss.str().size());
  val[*len] = '\0';

  return GRIB_SUCCESS;
}

long TimeSpan::get_native_type()
{
    return GRIB_TYPE_STRING;
}

}  // namespace eccodes::accessor

