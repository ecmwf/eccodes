/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G1FcPeriod.h"

eccodes::accessor::G1FcPeriod _grib_accessor_g1fcperiod;
eccodes::Accessor* grib_accessor_g1fcperiod = &_grib_accessor_g1fcperiod;

namespace eccodes::accessor
{

int G1FcPeriod::unpack_string(char* val, size_t* len)
{
    long start = 0, theEnd = 0;
    char tmp[1024];
    const size_t tmpLen = sizeof(tmp);
    int err             = grib_g1_step_get_steps(&start, &theEnd);
    size_t l            = 0;

    if (err)
        return err;

    snprintf(tmp, tmpLen, "%ld-%ld", start / 24, theEnd / 24);
    /*printf("---- FCPERIOD %s [start:%g, end:%g]",tmp,start,end);*/

    l = strlen(tmp) + 1;
    if (*len < l) {
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    *len = l;
    strcpy(val, tmp); /* NOLINT: CWE-119 clang-analyzer-security.insecureAPI.strcpy */

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
