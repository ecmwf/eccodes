/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G1Section4Length.h"

eccodes::accessor::G1Section4Length _grib_accessor_g1_section4_length;
eccodes::Accessor* grib_accessor_g1_section4_length = &_grib_accessor_g1_section4_length;

namespace eccodes::accessor
{

void G1Section4Length::init(const long len, grib_arguments* args)
{
    SectionLength::init(len, args);
    total_length_ = args->get_name(get_enclosing_handle(), 0);
}

int G1Section4Length::pack_long(const long* val, size_t* len)
{
    // Old implementation:
    //   Here we assume that the totalLength will be coded AFTER the section4 length, and
    //   the section4 length will be overwritten by the totalLength accessor for large GRIBs
    //    grib_accessor* super = *(cclass_ ->super);
    //    return super->pack_long(a,val,len);

    // Note: Do not directly call pack_long on base class
    // because in this special case we want to skip the checks.
    // So we call the helper function which has an extra argument
    return pack_long_unsigned_helper(val, len, /*check=*/0);
}

int G1Section4Length::unpack_long(long* val, size_t* len)
{
    int ret           = 0;
    long total_length = 0, sec4_length = 0;

    if ((ret = grib_get_g1_message_size(get_enclosing_handle(),
                                        grib_find_accessor(get_enclosing_handle(), total_length_),
                                        this,
                                        &total_length,
                                        &sec4_length)) != GRIB_SUCCESS) {
        return ret;
    }

    *val = sec4_length;

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
