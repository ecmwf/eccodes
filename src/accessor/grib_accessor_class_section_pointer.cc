
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_section_pointer.h"

grib_accessor_class_section_pointer_t _grib_accessor_class_section_pointer{"section_pointer"};
grib_accessor_class* grib_accessor_class_section_pointer = &_grib_accessor_class_section_pointer;


void grib_accessor_class_section_pointer_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_gen_t::init(a, len, arg);
    int n = 0;
    grib_accessor_section_pointer_t* self = (grib_accessor_section_pointer_t*)a;

    self->sectionOffset = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    self->sectionLength = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    self->sectionNumber = grib_arguments_get_long(grib_handle_of_accessor(a), arg, n++);

    Assert(self->sectionNumber < MAX_NUM_SECTIONS);

    grib_handle_of_accessor(a)->section_offset[self->sectionNumber] = (char*)self->sectionOffset;
    grib_handle_of_accessor(a)->section_length[self->sectionNumber] = (char*)self->sectionLength;

    /* printf("++++++++++++++ GRIB_API:  creating section_pointer%d %s %s\n", */
    /* self->sectionNumber,self->sectionLength,self->sectionLength); */

    if (grib_handle_of_accessor(a)->sections_count < self->sectionNumber)
        grib_handle_of_accessor(a)->sections_count = self->sectionNumber;

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_HIDDEN;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    a->length = 0;
}

int grib_accessor_class_section_pointer_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_BYTES;
}

int grib_accessor_class_section_pointer_t::unpack_string(grib_accessor* a, char* v, size_t* len){
    /*
      unsigned char* p=NULL;
      char* s=v;
      int i;
      long length=a->byte_count();
      if (*len < length) return GRIB_ARRAY_TOO_SMALL;

      p  = grib_handle_of_accessor(a)->buffer->data + a->byte_offset();
      for (i = 0; i < length; i++)  {
        snprintf (s,64,"%02x", *(p++));
        s+=2;
      }
      *len=length;
    */
    snprintf(v, 64, "%ld_%ld", a->byte_offset(), a->byte_count());    return GRIB_SUCCESS;
}

long grib_accessor_class_section_pointer_t::byte_count(grib_accessor* a){
    grib_accessor_section_pointer_t* self = (grib_accessor_section_pointer_t*)a;
    long sectionLength = 0;
    int ret = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->sectionLength, &sectionLength);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Unable to get %s %s",
                         self->sectionLength, grib_get_error_message(ret));
        return -1;
    }

    return sectionLength;
}

long grib_accessor_class_section_pointer_t::byte_offset(grib_accessor* a){
    grib_accessor_section_pointer_t* self = (grib_accessor_section_pointer_t*)a;
    long sectionOffset = 0;
    int ret = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->sectionOffset, &sectionOffset);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Unable to get %s %s",
                         self->sectionOffset, grib_get_error_message(ret));
        return -1;
    }

    return sectionOffset;
}
