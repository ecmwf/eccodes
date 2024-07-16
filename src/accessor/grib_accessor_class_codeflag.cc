
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_codeflag.h"

grib_accessor_class_codeflag_t _grib_accessor_class_codeflag{ "codeflag" };
grib_accessor_class* grib_accessor_class_codeflag = &_grib_accessor_class_codeflag;


void grib_accessor_class_codeflag_t::init(grib_accessor* a, const long len, grib_arguments* param)
{
    grib_accessor_class_unsigned_t::init(a, len, param);
    grib_accessor_codeflag_t* self = (grib_accessor_codeflag_t*)a;
    a->length = len;
    self->tablename = grib_arguments_get_string(grib_handle_of_accessor(a), param, 0);
    Assert(a->length >= 0);
}

static int test_bit(long a, long b)
{
    DEBUG_ASSERT(b >= 0);
    return a & (1 << b);
}

static int grib_get_codeflag(grib_accessor* a, long code, char* codename)
{
    const grib_accessor_codeflag_t* self = (grib_accessor_codeflag_t*)a;
    FILE* f = NULL;
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
        strncpy(fname, self->tablename, sizeof(fname) - 1);
        fname[sizeof(fname) - 1] = '\0';
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
                //if (j)
                //    codename[j++] = ' ';

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

    strcat(codename, " (");
    strcat(codename, fname);
    strcat(codename, ")");

    fclose(f);
    return GRIB_SUCCESS;
}

int grib_accessor_class_codeflag_t::value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

void grib_accessor_class_codeflag_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    const grib_accessor_codeflag_t* self = (grib_accessor_codeflag_t*)a;
    long v = 0;
    char flagname[1024] = {0,};
    char fname[1024] = {0,};

    size_t llen = 1;

    grib_recompose_name(grib_handle_of_accessor(a), NULL, self->tablename, fname, 1);
    a->unpack_long(&v, &llen);
    grib_get_codeflag(a, v, flagname);

    grib_dump_bits(dumper, a, flagname);
}
