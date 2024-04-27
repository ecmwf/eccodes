
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_unexpanded_descriptors.h"

grib_accessor_class_unexpanded_descriptors_t _grib_accessor_class_unexpanded_descriptors{"unexpanded_descriptors"};
grib_accessor_class* grib_accessor_class_unexpanded_descriptors = &_grib_accessor_class_unexpanded_descriptors;


void grib_accessor_class_unexpanded_descriptors_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_long_t::init(a, len, args);
    grib_accessor_unexpanded_descriptors_t* self = (grib_accessor_unexpanded_descriptors_t*)a;
    int n                                      = 0;
    grib_handle* hand                          = grib_handle_of_accessor(a);
    self->unexpandedDescriptorsEncoded         = grib_find_accessor(hand, grib_arguments_get_name(hand, args, n++));
    self->createNewData                        = grib_arguments_get_name(hand, args, n++);
    a->length                                  = 0;
}

int grib_accessor_class_unexpanded_descriptors_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_unexpanded_descriptors_t* self = (grib_accessor_unexpanded_descriptors_t*)a;
    int ret                                    = 0;
    long pos                                   = 0;
    long rlen                                  = 0;
    long f, x, y;
    long* v = val;
    long i;
    grib_handle* hand = grib_handle_of_accessor(a);

    pos = accessor_raw_get_offset(self->unexpandedDescriptorsEncoded) * 8;

    ret = value_count(a, &rlen);
    if (ret)
        return ret;

    if (rlen == 0) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "No descriptors in section 3. Malformed message.");
        return GRIB_MESSAGE_MALFORMED;
    }

    if (*len < rlen) {
        /* grib_context_log(a->context, GRIB_LOG_ERROR, */
        /* " wrong size (%ld) for %s it contains %d values ",*len, a->name , rlen); */
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < rlen; i++) {
        f    = grib_decode_unsigned_long(hand->buffer->data, &pos, 2);
        x    = grib_decode_unsigned_long(hand->buffer->data, &pos, 6);
        y    = grib_decode_unsigned_long(hand->buffer->data, &pos, 8);
        *v++ = f * 100000 + x * 1000 + y;
    }
    *len = rlen;
    return GRIB_SUCCESS;
}

int grib_accessor_class_unexpanded_descriptors_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_unexpanded_descriptors_t* self = (grib_accessor_unexpanded_descriptors_t*)a;
    int ret                                    = 0;
    long pos                                   = 0;
    unsigned long f, x, y;
    unsigned char* buf      = NULL;
    grib_accessor* expanded = NULL;
    size_t buflen           = *len * 2;
    size_t i = 0, length = *len;
    long createNewData = 1;
    grib_handle* hand  = grib_handle_of_accessor(a);

    grib_get_long(hand, self->createNewData, &createNewData);

    buf = (unsigned char*)grib_context_malloc_clear(a->context, buflen);

    for (i = 0; i < length; i++) {
        const long tmp = val[i] % 100000;
        f              = val[i] / 100000;
        x              = tmp / 1000;
        y              = tmp % 1000;
        grib_encode_unsigned_longb(buf, f, &pos, 2);
        grib_encode_unsigned_longb(buf, x, &pos, 6);
        grib_encode_unsigned_longb(buf, y, &pos, 8);
    }

    self->unexpandedDescriptorsEncoded->pack_bytes(buf, &buflen);    grib_context_free(hand->context, buf);

    if (createNewData == 0)
        return ret;

    expanded = grib_find_accessor(hand, "expandedCodes");
    Assert(expanded != NULL);
    ret = grib_accessor_class_expanded_descriptors_set_do_expand(expanded, 1);
    if (ret != GRIB_SUCCESS)
        return ret;

    ret = grib_set_long(hand, "unpack", 3); /* BUFR new data */
    if (ret != GRIB_SUCCESS)
        return ret;

    ret = grib_set_long(hand, "unpack", 1); /* Unpack structure */

    return ret;
}

int grib_accessor_class_unexpanded_descriptors_t::value_count(grib_accessor* a, long* numberOfUnexpandedDescriptors){
    grib_accessor_unexpanded_descriptors_t* self = (grib_accessor_unexpanded_descriptors_t*)a;
    long n                                     = 0;

    self->unexpandedDescriptorsEncoded->value_count(&n);    *numberOfUnexpandedDescriptors = n / 2;

    return 0;
}

long grib_accessor_class_unexpanded_descriptors_t::byte_offset(grib_accessor* a){
    return a->offset;
}

void grib_accessor_class_unexpanded_descriptors_t::update_size(grib_accessor* a, size_t s){
    a->length = s;
}

long grib_accessor_class_unexpanded_descriptors_t::next_offset(grib_accessor* a){
    return a->byte_offset() + a->length;
}
