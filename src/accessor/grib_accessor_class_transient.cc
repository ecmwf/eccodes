/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_transient.h"

grib_accessor_class_transient_t _grib_accessor_class_transient{ "transient" };
grib_accessor_class* grib_accessor_class_transient = &_grib_accessor_class_transient;


void grib_accessor_class_transient_t::init(grib_accessor* a, const long l, grib_arguments* args)
{
    grib_accessor_class_variable_t::init(a, l, args);
}
