/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/****************************************
 * Enrico Fucile
 **************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_unsigned
   IMPLEMENTS = init;dump
   IMPLEMENTS = value_count
   MEMBERS    = const char* tablename
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int value_count(grib_accessor*, long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_codeflag
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in unsigned */
    long nbytes;
    grib_arguments* arg;
    /* Members defined in codeflag */
    const char* tablename;
} grib_accessor_codeflag;

extern grib_accessor_class* grib_accessor_class_unsigned;

static grib_accessor_class _grib_accessor_class_codeflag = {
    &grib_accessor_class_unsigned,                      /* super */
    "codeflag",                      /* name */
    sizeof(grib_accessor_codeflag),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
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
    0,              /* unpack_string */
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


grib_accessor_class* grib_accessor_class_codeflag = &_grib_accessor_class_codeflag;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* param)
{
    grib_accessor_codeflag* self = (grib_accessor_codeflag*)a;
    a->length                    = len;
    self->tablename              = grib_arguments_get_string(grib_handle_of_accessor(a), param, 0);
    Assert(a->length >= 0);
}

static int test_bit(long a, long b)
{
    DEBUG_ASSERT(b>=0);
    return a & (1 << b);
}

static int grib_get_codeflag(grib_accessor* a, long code, char* codename)
{
    grib_accessor_codeflag* self = (grib_accessor_codeflag*)a;
    FILE* f                      = NULL;
    char fname[1024];
    char bval[50];
    char num[50];
    char* filename = 0;
    char line[1024];
    size_t i = 0;
    int j    = 0;
    int err  = 0;

    err = grib_recompose_name(grib_handle_of_accessor(a), NULL, self->tablename, fname, 1);
    if (err) {
        strncpy(fname, self->tablename, sizeof(fname)-1);
        fname[sizeof(fname)-1] = '\0';
    }

    if ((filename = grib_context_full_defs_path(a->context, fname)) == NULL) {
        grib_context_log(a->context, GRIB_LOG_WARNING, "Cannot open flag table %s", filename);
        strcpy(codename, "Cannot open flag table");
        return GRIB_FILE_NOT_FOUND;
    }

    f = codes_fopen(filename, "r");
    if (!f) {
        grib_context_log(a->context, (GRIB_LOG_WARNING) | (GRIB_LOG_PERROR), "Cannot open flag table %s", filename);
        strcpy(codename, "Cannot open flag table");
        return GRIB_FILE_NOT_FOUND;
    }

    // strcpy(codename, self->tablename);
    // strcat(codename,": ");
    // j = strlen(codename);

    while (fgets(line, sizeof(line) - 1, f)) {
        sscanf(line, "%49s %49s", num, bval);

        if (num[0] != '#') {
            if ((test_bit(code, a->length * 8 - atol(num)) > 0) == atol(bval)) {
                size_t linelen = strlen(line);
                codename[j++]  = '(';
                codename[j++]  = num[0];
                codename[j++]  = '=';
                codename[j++]  = bval[0];
                codename[j++]  = ')';
                codename[j++]  = ' ';
                if (j)
                    codename[j++] = ' ';

                for (i = (strlen(num) + strlen(bval) + 2); i < linelen - 1; i++)
                    codename[j++] = line[i];
                if (line[i] != '\n')
                    codename[j++] = line[i];
                codename[j++] = ';';
            }
        }
    }

    if (j > 1 && codename[j - 1] == ';')
        j--;
    codename[j] = 0;

    strcat(codename, ":");
    strcat(codename, fname);

    fclose(f);
    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_accessor_codeflag* self = (grib_accessor_codeflag*)a;
    long v;
    char flagname[1024];
    char fname[1024];

    size_t llen = 1;

    grib_recompose_name(grib_handle_of_accessor(a), NULL, self->tablename, fname, 1);
    grib_unpack_long(a, &v, &llen);

    grib_get_codeflag(a, v, flagname);

    grib_dump_bits(dumper, a, flagname);
}
