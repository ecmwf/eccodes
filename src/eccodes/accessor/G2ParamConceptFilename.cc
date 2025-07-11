/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G2ParamConceptFilename.h"

eccodes::accessor::G2ParamConceptFilename _grib_accessor_g2_param_concept_filename{};
eccodes::Accessor* grib_accessor_g2_param_concept_filename = &_grib_accessor_g2_param_concept_filename;

namespace eccodes::accessor
{

void G2ParamConceptFilename::init(const long len, grib_arguments* arg)
{
    grib_handle* h = get_enclosing_handle();

    if (context_->debug) {
        const int numActualArgs   = arg->get_count();
        const int numExpectedArgs = 3;
        if (numActualArgs != numExpectedArgs) {
            grib_context_log(context_, GRIB_LOG_FATAL, "Accessor %s (key %s): %d arguments provided but expected %d",
                             class_name_, name_, numActualArgs, numExpectedArgs);
        }
    }

    basename_                = arg->get_string(h, 0);
    MTG2Switch_              = arg->get_name(h, 1);
    tablesVersionMTG2Switch_ = arg->get_name(h, 2);

    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

long G2ParamConceptFilename::get_native_type()
{
    return GRIB_TYPE_STRING;
}

// If MTG2Switch is 0 (pre-MTG2) we need to look at pre-MTG2 definitions using the tablesVersionMTG2Switch key.
// Otherwise look at new definitions.
int G2ParamConceptFilename::unpack_string(char* v, size_t* len)
{
    grib_handle* h = get_enclosing_handle();
    int err = 0;
    long MTG2Switch = 0, tablesVersionMTG2Switch = 0;

    err = grib_get_long_internal(h, MTG2Switch_, &MTG2Switch);
    if (err) return err;
    err = grib_get_long_internal(h, tablesVersionMTG2Switch_, &tablesVersionMTG2Switch);
    if (err) return err;

    const size_t dsize = string_length() - 1; // size for destination string "v"
    if ( MTG2Switch == 0 ) {
        snprintf(v, dsize, "%s.lte%ld.def", basename_, tablesVersionMTG2Switch);
    } else if ( MTG2Switch == 2 ) {
        snprintf(v, dsize, "%s.chemsplit.def", basename_); 
    } else {
        // All other cases other than pre-MTG2 fall into default parameter files
        snprintf(v, dsize, "%s.def", basename_);
    }

    size_t size = strlen(v);
    ECCODES_ASSERT(size > 0);
    *len = size + 1;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
