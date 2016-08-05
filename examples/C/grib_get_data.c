/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_get_data
 *
 * Description: how to get lat/lon/values from a GRIB message
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "eccodes.h"

int main (int argc, char **argv)
{
    int err = 0;
    size_t i = 0;
    FILE *in = NULL;
    const char *filename = "../../data/reduced_latlon_surface.grib1";
    codes_handle *h = NULL;
    long numberOfPoints = 0;
    const double missing = 9999.0;
    double *lats, *lons, *values;       /* arrays */

    in = fopen (filename, "r");
    if (!in) {
        printf ("ERROR: unable to open input file %s\n", filename);
        return 1;
    }

    /* create new handle from a message in a file */
    h = codes_handle_new_from_file (0, in, PRODUCT_GRIB, &err);
    if (h == NULL) {
        printf ("Error: unable to create handle from file %s\n", filename);
        return 1;
    }

    CODES_CHECK (codes_get_long (h, "numberOfPoints", &numberOfPoints), 0);
    CODES_CHECK (codes_set_double (h, "missingValue", missing), 0);

    lats = (double *) malloc (numberOfPoints * sizeof (double));
    if (!lats) {
        printf ("unable to allocate %ld bytes\n", (long) (numberOfPoints * sizeof (double)));
        return 1;
    }
    lons = (double *) malloc (numberOfPoints * sizeof (double));
    if (!lons) {
        printf ("unable to allocate %ld bytes\n", (long) (numberOfPoints * sizeof (double)));
        free (lats);
        return 1;
    }
    values = (double *) malloc (numberOfPoints * sizeof (double));
    if (!values) {
        printf ("unable to allocate %ld bytes\n", (long) (numberOfPoints * sizeof (double)));
        free (lats);
        free (lons);
        return 1;
    }

    CODES_CHECK (codes_grib_get_data (h, lats, lons, values), 0);

    for (i = 0; i < numberOfPoints; ++i) {
        if (values[i] != missing) {
            printf ("%f %f %f\n", lats[i], lons[i], values[i]);
        }
    }

    free (lats);
    free (lons);
    free (values);
    codes_handle_delete (h);

    fclose (in);
    return 0;
}
