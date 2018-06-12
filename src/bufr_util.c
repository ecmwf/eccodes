/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/* Return the rank of the key using list of keys (For BUFR keys) */
/* The argument 'keys' is an input as well as output from each call */
int compute_bufr_key_rank(grib_handle* h, grib_string_list* keys, const char* key)
{
    grib_string_list* next=keys;
    grib_string_list* prev=keys;
    int theRank=0;
    size_t size=0;
    grib_context* c=h->context;
    DebugAssert(h->product_kind == PRODUCT_BUFR);

    while (next && next->value && strcmp(next->value,key)) {
        prev=next;
        next=next->next;
    }
    if (!next) {
        prev->next=(grib_string_list*)grib_context_malloc_clear(c,sizeof(grib_string_list));
        next=prev->next;
    }
    if (!next->value) {
        next->value=strdup(key);
        next->count=0;
    }

    next->count++;
    theRank=next->count;
    if (theRank==1) {
        /* If the count is 1 it could mean two things: */
        /*   This is the first instance of the key and there is another one */
        /*   This is the first and only instance of the key */
        /* So we check if there is a second one of this key, */
        /* If not, then rank is zero i.e. this is the only instance */
        char* s=(char*)grib_context_malloc_clear(c,strlen(key)+5);
        sprintf(s,"#2#%s",key);
        if (grib_get_size(h,s,&size)==GRIB_NOT_FOUND) theRank=0;
        grib_context_free(c, s);
    }

    return theRank;
}

char** codes_bufr_copy_data_return_copied_keys(grib_handle* hin,grib_handle* hout, size_t* nkeys, int* err)
{
    bufr_keys_iterator* kiter=NULL;
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
        if (*err==0) {
            /* 'name' will be freed when we call codes_bufr_keys_iterator_delete so copy */
            char* copied_name = strdup(name);
            k=grib_sarray_push(hin->context, k, copied_name);
        }
    }
    *nkeys=grib_sarray_used_size(k);
    keys=grib_sarray_get_array(hin->context, k);
    grib_sarray_delete(hin->context, k);
    if (*nkeys > 0) {
        /* Do the pack if something was copied */
        *err=grib_set_long(hout, "pack", 1);
    }
    codes_bufr_keys_iterator_delete(kiter);
    return keys;
}

int codes_bufr_copy_data(grib_handle* hin, grib_handle* hout)
{
    bufr_keys_iterator* kiter=NULL;
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
    }

    if (nkeys > 0) {
        /* Do the pack if something was copied */
        err=grib_set_long(hout, "pack", 1);
    }

    codes_bufr_keys_iterator_delete(kiter);
    return err;
}
