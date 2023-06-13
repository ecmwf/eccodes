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
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init;get_native_type;unpack_string
   IMPLEMENTS = byte_count; byte_offset
   MEMBERS = const char* sectionOffset
   MEMBERS = const char* sectionLength
   MEMBERS = long sectionNumber
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int unpack_string(grib_accessor*, char*, size_t* len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_section_pointer
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in section_pointer */
    const char* sectionOffset;
    const char* sectionLength;
    long sectionNumber;
} grib_accessor_section_pointer;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_section_pointer = {
    &grib_accessor_class_gen,                      /* super */
    "section_pointer",                      /* name */
    sizeof(grib_accessor_section_pointer),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    &byte_count,                 /* get number of bytes */
    &byte_offset,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    &unpack_string,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_section_pointer = &_grib_accessor_class_section_pointer;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    int n                               = 0;
    grib_accessor_section_pointer* self = (grib_accessor_section_pointer*)a;

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

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_BYTES;
}

static int unpack_string(grib_accessor* a, char* v, size_t* len)
{
    /*
      unsigned char* p=NULL;
      char* s=v;
      int i;
      long length=grib_byte_count(a);

      if (*len < length) return GRIB_ARRAY_TOO_SMALL;

      p  = grib_handle_of_accessor(a)->buffer->data + grib_byte_offset(a);

      for (i = 0; i < length; i++)  {
        snprintf (s,64,"%02x", *(p++));
        s+=2;
      }
      *len=length;
    */
    snprintf(v, 64, "%ld_%ld", grib_byte_offset(a), grib_byte_count(a));
    return GRIB_SUCCESS;
}

static long byte_count(grib_accessor* a)
{
    grib_accessor_section_pointer* self = (grib_accessor_section_pointer*)a;
    long sectionLength                  = 0;
    int ret                             = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->sectionLength, &sectionLength);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to get %s %s",
                         self->sectionLength, grib_get_error_message(ret));
        return -1;
    }

    return sectionLength;
}

static long byte_offset(grib_accessor* a)
{
    grib_accessor_section_pointer* self = (grib_accessor_section_pointer*)a;
    long sectionOffset                  = 0;
    int ret                             = 0;

    ret = grib_get_long(grib_handle_of_accessor(a), self->sectionOffset, &sectionOffset);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to get %s %s",
                         self->sectionOffset, grib_get_error_message(ret));
        return -1;
    }

    return sectionOffset;
}
