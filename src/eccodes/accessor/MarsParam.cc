/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "MarsParam.h"

eccodes::AccessorBuilder<eccodes::accessor::MarsParam> _grib_accessor_mars_param_builder{};

namespace eccodes::accessor
{

void MarsParam::init(const long l, grib_arguments* c)
{
    Ascii::init(l, c);
    int n    = 0;
    paramId_ = c->get_name(get_enclosing_handle(), n++);
    table_   = c->get_name(get_enclosing_handle(), n++);
    param_   = c->get_name(get_enclosing_handle(), n++);
}

// For an implementation of pack_string, see
//   src/deprecated/grib_accessor_mars_param.cc

// For an alternative implementation of unpack_string, see
//   src/deprecated/grib_accessor_mars_param.cc
//
int MarsParam::unpack_string(char* val, size_t* len)
{
    long param = 0;
    long table = 0;
    int ret    = 0;

    if (table_ != NULL && (ret = grib_get_long_internal(get_enclosing_handle(), table_, &table)) != GRIB_SUCCESS)
        return ret;
    if (param_ != NULL && (ret = grib_get_long_internal(get_enclosing_handle(), param_, &param)) != GRIB_SUCCESS)
        return ret;

    /*if (table==200) table=128;*/
    snprintf(val, 32, "%ld.%ld", param, table);
    *len = strlen(val) + 1;

    return GRIB_SUCCESS;
}

size_t MarsParam::string_length()
{
    return 7;
}

}  // namespace eccodes::accessor
