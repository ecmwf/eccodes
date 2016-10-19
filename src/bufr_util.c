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

char** codes_bufr_copy_data_return_copied_keys(grib_handle* hin,grib_handle* hout, size_t* nkeys, int* err)
{
    grib_keys_iterator* kiter=NULL;
    char* name=0;
    char** keys=NULL;
    grib_sarray* k=0;

    if (hin==NULL || hout==NULL) {
        *err = GRIB_NULL_HANDLE;
        return NULL;
    }

    kiter=codes_bufr_data_section_keys_iterator_new(hin);
    if (!kiter) return NULL;
    k=grib_sarray_new(hin->context, 50, 10);

    while(codes_bufr_keys_iterator_next(kiter))
    {
        name = codes_bufr_keys_iterator_get_name(kiter);
        /* if the copy fails we want to keep copying without any errors.
           This is because the copy can be between structures that are not
           identical and we want to copy what can be copied and skip what
           cannot be copied because is not in the output handle
         */
        *err=codes_copy_key(hin, hout, name, 0);
        if (*err==0) k=grib_sarray_push(hin->context, k, name);
    }
    *nkeys=grib_sarray_used_size(k);
    keys=grib_sarray_get_array(hin->context, k);
    grib_sarray_delete(hin->context, k);
    if (*nkeys > 0) {
        /* Do the pack if something was copied */
        *err=grib_set_long(hout, "pack", 1);
    }
    grib_keys_iterator_delete(kiter);
    return keys;
}

int codes_bufr_copy_data(grib_handle* hin, grib_handle* hout)
{
    grib_keys_iterator* kiter=NULL;
    char* name=0;
    int err=0;
    int nkeys=0;

    if (hin==NULL || hout==NULL) {
        return GRIB_NULL_HANDLE;
    }

    kiter=codes_bufr_data_section_keys_iterator_new(hin);
    if (!kiter) return GRIB_INTERNAL_ERROR;

    while(codes_bufr_keys_iterator_next(kiter))
    {
        name = codes_bufr_keys_iterator_get_name(kiter);
        /* if the copy fails we want to keep copying without any error messages.
           This is because the copy can be between structures that are not
           identical and we want to copy what can be copied and skip what
           cannot be copied because is not in the output handle
         */
        err=codes_copy_key(hin, hout, name, 0);
        if (err==0) nkeys++;
        grib_context_free(hin->context,name);
    }

    if (nkeys > 0) {
        /* Do the pack if something was copied */
        err=grib_set_long(hout, "pack", 1);
    }

    grib_keys_iterator_delete(kiter);
    return err;
}
