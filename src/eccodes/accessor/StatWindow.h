/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once


#include "Gen.h"


namespace eccodes::accessor
{

class StatWindow : public Gen
{
public:
    StatWindow() : Gen() { class_name_ = "statwindow"; }
    grib_accessor* create_empty_accessor() override { return new StatWindow{}; }
    void init(const long, grib_arguments *) override;
    int unpack_string(char *val, size_t *len) override;
    long get_native_type() override;

private:
  const char* time_range_key_;
  const char* unit_range_key_;
};

}  // namespace eccodes::accessor
