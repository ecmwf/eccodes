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
#include "geo/iterator/Iterator.h"

namespace eccodes::accessor
{

class Iterator : public Gen
{
public:
    Iterator() :
        Gen() { class_name_ = "iterator"; }
    grib_accessor* create_empty_accessor() override { return new Iterator{}; }
    void init(const long l, grib_arguments* args) override;
    void dump(eccodes::Dumper* dumper) override;

private:
    grib_arguments* args_ = nullptr;
    friend eccodes::geo_iterator::Iterator* eccodes::geo_iterator::gribIteratorNew(const grib_handle*, unsigned long, int*);
};

}  // namespace eccodes::accessor
