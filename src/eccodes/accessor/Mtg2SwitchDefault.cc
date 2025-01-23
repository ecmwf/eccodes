/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Mtg2SwitchDefault.h"

grib_accessor_mtg2_switch_default_t _grib_accessor_mtg2_switch_default{};
grib_accessor* grib_accessor_mtg2_switch_default = &_grib_accessor_mtg2_switch_default;

namespace eccodes::accessor
{

void grib_accessor_mtg2_switch_default_t::init(const long len, grib_arguments* arg)
{
    grib_accessor_long_t::init(len, arg);

    grib_handle* h = grib_handle_of_accessor(this);

    this->tablesVersion_           = arg->get_name(h, 0);
    this->tablesVersionMTG2Switch_ = arg->get_name(h, 1);
    this->marsClass_               = arg->get_name(h, 2);

    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

int grib_accessor_mtg2_switch_default_t::unpack_long(long* val, size_t* len)
{
    grib_handle* h = grib_handle_of_accessor(this);
    int err = 0;
    long tablesVersion = 0, tablesVersionMTG2Switch = 0;
    char marsClass[32] = {0,};

    err = grib_get_long(h, tablesVersion_, &tablesVersion);
    if (err) return err;
    err = grib_get_long(h, tablesVersionMTG2Switch_, &tablesVersionMTG2Switch);
    if (err) return err;
    size_t size = sizeof(marsClass);

    bool marsClassExists = true;
    err = grib_get_string(h, marsClass_, marsClass, &size);
    if (err) {
        if (err == GRIB_NOT_FOUND) {
            marsClassExists = false;
            err = 0;
        }
        else {
            return err;
        }
    }

    if (tablesVersion <= tablesVersionMTG2Switch) {
        *val = 0; // Pre-MTG2
    } else {
        // For class mc and cr post MTG2 we always want the param + chem split (value 2)
        // For TIGGE, marsClass is not defined in the empty local Section 2, but is defined later on.
        if ( marsClassExists && (STR_EQUAL(marsClass, "mc") || STR_EQUAL(marsClass, "cr")) ) {
            *val = 2;
        } else {
            *val = 1; // All other cases we are post-MTG2
        }
    }

    return err;
}

}  // namespace eccodes::accessor
