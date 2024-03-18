
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_check_internal_version.h"

grib_accessor_class_check_internal_version_t _grib_accessor_class_check_internal_version{"check_internal_version"};
grib_accessor_class* grib_accessor_class_check_internal_version = &_grib_accessor_class_check_internal_version;


/* This is the internal engine version number */
/* We check this against the version number found in the definitions boot.def file */
/* See the key "internalVersion"  */
#define LATEST_ENGINE_VERSION 30

void grib_accessor_class_check_internal_version_t::init(grib_accessor* a, const long l, grib_arguments* args){
    /* Check version of definition files is compatible with the engine */
    int err                    = 0;
    long defs_file_version     = 0;
    grib_handle* h             = grib_handle_of_accessor(a);
    const char* s_defn_version = grib_arguments_get_name(h, args, 0);
    Assert(s_defn_version);

    err = grib_get_long_internal(h, s_defn_version, &defs_file_version);
    if (!err) {
        if (defs_file_version > LATEST_ENGINE_VERSION) {
            grib_context_log(h->context, GRIB_LOG_FATAL,
                             "Definition files version (%d) is greater than engine version (%d)!\n"
                             "                    " /* indent for 2nd line */
                             "These definition files are for a later version of the ecCodes engine.",
                             defs_file_version, LATEST_ENGINE_VERSION);
        }
    }
}

int grib_accessor_class_check_internal_version_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}

size_t grib_accessor_class_check_internal_version_t::string_length(grib_accessor* a){
    return 255;
}
