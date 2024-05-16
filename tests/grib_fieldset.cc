/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grib_api.h"

int main(int argc, char** argv)
{
    int err = 0;
    long step;
    const char** filenames = NULL;
    size_t nkeys, nfiles, i=0;
    const char* keys[] = { "step:i", "date", "paramId", "levelType" };
    grib_fieldset* set = NULL;
    grib_handle* h = NULL;
    char param[20] = {0,};
    char typeOfLevel[50] = {0,};
    char date[10] = {0,};
    size_t lenDate  = 10, lenParam = 20, lenLevel = 50;
    char* order_by  = NULL;

    if (argc != 3) return 1; //Usage: prog order_by grib_file grib_file ...

    nkeys    = sizeof(keys) / sizeof(*keys);
    order_by = argv[1];

    nfiles    = argc - 2;
    filenames = (const char**)malloc(sizeof(char*) * nfiles);
    for (i = 0; i < nfiles; i++)
        filenames[i] = (char*)strdup(argv[i + 2]);

    set = grib_fieldset_new_from_files(0, filenames, nfiles, keys, nkeys, 0, 0, &err);
    GRIB_CHECK(err, 0);

    /* not yet implemented */
    /* err=grib_fieldset_apply_where(set,"(centre=='ecmf') && number==1 || step==6 "); */
    /* GRIB_CHECK(err,0); */

    grib_fieldset_apply_order_by(set, order_by);
    GRIB_CHECK(err, 0);

    printf("Ordering by %s\n", order_by);
    printf("\n%d fields in the fieldset\n", grib_fieldset_count(set));
    printf("step,date,levelType,levelType\n");
    while ((h = grib_fieldset_next_handle(set, &err)) != NULL) {
        lenParam = sizeof(param);
        lenDate  = sizeof(date);
        lenLevel = sizeof(typeOfLevel);
        GRIB_CHECK(grib_get_long(h, "step", &step), 0);
        GRIB_CHECK(grib_get_string(h, "date", date, &lenDate), 0);
        GRIB_CHECK(grib_get_string(h, "paramId", param, &lenParam), 0);
        GRIB_CHECK(grib_get_string(h, "levelType", typeOfLevel, &lenLevel), 0);

        printf("%ld %s %s %s\n", step, date, typeOfLevel, param);
        grib_handle_delete(h);
    }

    grib_fieldset_delete(set);
    grib_handle_delete(h);
    for (i = 0; i < nfiles; i++)
        free((char*)filenames[i]);
    free(filenames);

    return 0;
}
