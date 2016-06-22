/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#ifdef HAVE_MEMFS
int codes_memfs_exists(const char* path);
FILE* codes_memfs_open(const char* path);

FILE* codes_fopen(const char* name, const char *mode) {
    FILE *f;

    if (strcmp(mode, "r") != 0) {
        return fopen(name, mode);
    }

    f = codes_memfs_open(name);
    if (f) {
        return f;
    }

    return fopen(name, mode);
}

int codes_access(const char* name, int mode) {
    if (mode != F_OK) {
        return access(name, mode);
    }

    if (codes_memfs_exists(name)) {
        return 0;
    }

    return access(name, mode);
}

#else

FILE* codes_fopen(const char* name, const char* mode) {
    return fopen(name, mode);
}

int codes_access(const char* name, int mode) {
    return access(name, mode);
}

#endif

