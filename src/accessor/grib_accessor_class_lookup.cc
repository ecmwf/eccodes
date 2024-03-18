
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
#include "grib_accessor_class_lookup.h"

grib_accessor_class_lookup_t _grib_accessor_class_lookup{"lookup"};
grib_accessor_class* grib_accessor_class_lookup = &_grib_accessor_class_lookup;


void grib_accessor_class_lookup_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_lookup_t* self = (grib_accessor_lookup_t*)a;
    a->length                  = 0;
    self->llength              = len;
    self->loffset              = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 0);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    self->real_name = grib_arguments_get_expression(grib_handle_of_accessor(a), arg, 1);
}

void grib_accessor_class_lookup_t::post_init(grib_accessor* a){
    grib_accessor_lookup_t* self = (grib_accessor_lookup_t*)a;
    if (self->real_name) {
        grib_dependency_observe_expression(a, self->real_name);
    }
}

void grib_accessor_class_lookup_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_accessor_lookup_t* self = (grib_accessor_lookup_t*)a;
    unsigned char bytes[1024]  = {0,};
    char msg[1024] = {0,};
    char buf[2048];
    int i;
    unsigned long v = 0;

    size_t llen = self->llength;
    a->unpack_bytes(bytes, &llen);
    bytes[llen] = 0;
    for (i = 0; i < llen; i++) {
        msg[i] = isprint(bytes[i]) ? bytes[i] : '?';
        v <<= 8;
        v |= bytes[i];
    }

    msg[llen] = 0;

    snprintf(buf, sizeof(buf), "%s %lu %ld-%ld", msg, v, (long)a->offset + self->loffset, (long)self->llength);

    grib_dump_long(dumper, a, buf);
}

int grib_accessor_class_lookup_t::unpack_string(grib_accessor* a, char* v, size_t* len){
    grib_accessor_lookup_t* self = (grib_accessor_lookup_t*)a;
    unsigned char bytes[1024]  = {0,};
    int i;

    size_t llen = self->llength;
    a->unpack_bytes(bytes, &llen);
    bytes[llen] = 0;

    for (i = 0; i < llen; i++) {
        v[i] = isprint(bytes[i]) ? bytes[i] : '?';
    }

    v[llen] = 0;
    if (llen == 1 && v[0] == '?') {
        /* Try unpack as long */
        size_t length = 10;
        long lval     = 0;
        int err       = unpack_long(a, &lval, &length);
        if (!err) {
            char str[5];
            int conv = snprintf(str, sizeof(str), "%ld", lval);
            if (conv == 1) {
                v[0] = str[0];
            }
        }
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_lookup_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_lookup_t* self = (grib_accessor_lookup_t*)a;
    grib_handle* h           = grib_handle_of_accessor(a);

    long pos = (a->offset + self->loffset) * 8;

    if (len[0] < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    /* This is used when reparsing or rebuilding */
    if (h->loader) {
        Assert(*len == 1);
        return h->loader->lookup_long(h->context, h->loader, a->name, val);
    }

    val[0] = grib_decode_unsigned_long(h->buffer->data, &pos, self->llength * 8);
    len[0] = 1;

    /*printf("###########lookup unpack_long: %s %ld %ld\n",a->name, pos/8, val[0]);*/

    return GRIB_SUCCESS;
}

int grib_accessor_class_lookup_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

long grib_accessor_class_lookup_t::byte_count(grib_accessor* a){
    grib_accessor_lookup_t* self = (grib_accessor_lookup_t*)a;
    return self->llength;
}

long grib_accessor_class_lookup_t::byte_offset(grib_accessor* a){
    grib_accessor_lookup_t* self = (grib_accessor_lookup_t*)a;
    return self->loffset;
}

int grib_accessor_class_lookup_t::notify_change(grib_accessor* self, grib_accessor* changed){
    /* Forward changes */
    return grib_dependency_notify_change(self);
}
