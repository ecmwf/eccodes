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

eccodes::AccessorBuilder<eccodes::accessor::Mtg2SwitchDefault> _grib_accessor_mtg2_switch_default_builder{};

namespace eccodes::accessor
{

void Mtg2SwitchDefault::init(const long len, grib_arguments* arg)
{
    Long::init(len, arg);

    grib_handle* h = get_enclosing_handle();

    if (context_->debug) {
        const int numActualArgs   = arg->get_count();
        const int numExpectedArgs = 4;
        if (numActualArgs != numExpectedArgs) {
            grib_context_log(context_, GRIB_LOG_FATAL, "Accessor %s (key %s): %d arguments provided but expected %d",
                             accessor_type().get().c_str(), name_, numActualArgs, numExpectedArgs);
        }
    }

    tablesVersion_           = arg->get_name(h, 0);
    tablesVersionMTG2Switch_ = arg->get_name(h, 1);
    marsClass_               = arg->get_name(h, 2);
    MTG2SwitchViaTablesVersion_ = arg->get_name(h, 3);

    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

// MTG2 behaviour based on tablesVersion
// Without accessing this logic (i.e. no ECMWF Section 2) the default is 1 = post-MTG2
//  0 = pre-MTG2 encoding used
//  1 = post-MTG2 encoding used
//  2 = post-MTG2 encoding with paramId + chemId used
int Mtg2SwitchDefault::unpack_long(long* val, size_t* len)
{
    grib_handle* h     = get_enclosing_handle();
    int err            = 0;
    long tablesVersion = 0, tablesVersionMTG2Switch = 0;
    char marsClass[32] = {0,};

    err = grib_get_long(h, tablesVersion_, &tablesVersion);
    if (err) return err;
    err = grib_get_long_internal(h, tablesVersionMTG2Switch_, &tablesVersionMTG2Switch);
    if (err) return err;

    bool marsClassExists = true;
    size_t size = sizeof(marsClass);
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

    // This is a boolean
    long MTG2SwitchViaTablesVersion = 0;
    err = grib_get_long(h, MTG2SwitchViaTablesVersion_, &MTG2SwitchViaTablesVersion);
    if (err) return err;

    if (MTG2SwitchViaTablesVersion) {
        if (tablesVersion <= tablesVersionMTG2Switch) {
            *val = 0;  // Pre-MTG2
        }
        else {
            // For certain classes post MTG2 we always want the param + chem split (value 2)
            // For TIGGE, marsClass is not defined in the empty local Section 2, but is defined later on.
            if ( marsClassExists && (STR_EQUAL(marsClass, "mc") || STR_EQUAL(marsClass, "cr") || STR_EQUAL(marsClass, "a5")) ) {
//            if ( marsClassExists && (STR_EQUAL(marsClass, "mc") || STR_EQUAL(marsClass, "cr") || STR_EQUAL(marsClass, "a5")) && ( grib_is_defined(h, "constituentType") || grib_is_defined(h, "aerosolType")) ) {
                *val = 2;
            }
            else {
                *val = 1;  // Post-MTG2
            }
        }
    }
    else {
        *val = 1;  // Post-MTG2
    }

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
