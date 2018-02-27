/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api.h"

#define CMAP_MAX 20480

typedef struct map
{
    double min;
    double max;
    unsigned char r, g, b;
} map;

map cmap[CMAP_MAX];
unsigned long last[CMAP_MAX] = {0,};

/*
unsigned char unused(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 10;
    }
    return c - '0';
}
*/

static int next(FILE *f, char *buf)
{
    int c;
    int i = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (c == ' ' || c == '\n')
        {
            if (i)
            {
                break;
            }
            i++;
        }
        else
        {
            buf[i++] = c;
        }
    }
    buf[i] = 0;
    return i;
}

int main(int argc, char *argv[])
{
    grib_handle *h = NULL;
    FILE *f = NULL;
    int i = 0;
    int err = 0;
    int centred = 0;
    unsigned r, g, b;
    int cmap_entries = 0;
    double min, max;
    int j;
    char buf[1024];
    double lcap = -1e+100, ucap = 1e+100;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {

            case 'c':
                centred = 1;
                break;

            case 'u':
                ucap = atof(argv[i + 1]);
                i++;
                break;

            case 'l':
                lcap = atof(argv[i + 1]);
                i++;
                break;

            case 'm':
                f = fopen(argv[i + 1], "r");
                if (!f)
                {
                    perror(argv[i + 1]);
                    exit(1);
                }
                while (next(f, buf))
                {
                    min = atof(buf);
                    next(f, buf);
                    max = atof(buf);
                    next(f, buf);
                    r = atol(buf);
                    next(f, buf);
                    g = atol(buf); next(f, buf);
                    b = atol(buf);

                    if (cmap_entries >= CMAP_MAX)
                    {
                        fprintf(stderr, "%s\n", "colour map is too large");
                        exit(1);
                    }
                    cmap[cmap_entries].min = min;
                    cmap[cmap_entries].max = max;
                    cmap[cmap_entries].r = r;
                    cmap[cmap_entries].g = g;
                    cmap[cmap_entries].b = b;
                    cmap_entries++;
                }
                fclose(f);
                i++;
                fprintf(stderr, "Number of colours: %d\n", cmap_entries);
            }
            continue;
        }

        f = fopen(argv[i], "r");
        if (!f)
        {
            perror(argv[i]);
            exit(1);
        }

        while ((h = grib_handle_new_from_file(0, f, &err)) != NULL)
        {
            long width;
            long height;
            double max, min;
            double *values = NULL;
            unsigned long *indices;
            size_t count;

            GRIB_CHECK(grib_get_size(h, "values", &count), 0);
            values = (double *)malloc(sizeof(double) * count);
            indices = (unsigned long *)malloc(sizeof(unsigned long) * count);

            GRIB_CHECK(grib_get_long(h, "Ni", &width), 0);
            GRIB_CHECK(grib_get_long(h, "Nj", &height), 0);
            GRIB_CHECK(grib_get_double_array(h, "values", values, &count), 0);

            max = values[0];
            min = values[0];
            indices[0] = 0;
            for (i = 1; i < count; ++i)
            {
                if (values[i] > max)
                    max = values[i];
                if (values[i] < min)
                    min = values[i];
                indices[i] = i;
            }

            if (max > ucap)
            {
                max = ucap;
            }

            if (min < lcap)
            {
                min = lcap;
            }


            fprintf(stderr, "width=%ld, height=%ld, min=%g, max=%g\n", width, height, min, max);
            if (centred)
            {
                /* assume first column in Greenwich meridian
                assume scanning mode
                 */
                int k = 0, j;
                for (j = 0; j < height; j++)
                {
                    for (i = 0; i < width; i++)
                    {
                        int m = (i + width / 2) % width + j * width;
                        indices[k++] = m;
                    }
                }
            }

            if (cmap_entries)
            {
                printf("P6\n%ld %ld\n255\n", width, height);
                for (i = 0; i < count; ++i)
                {
                    double v = values[indices[i]];
                    unsigned int p = ((unsigned long)v) % CMAP_MAX; /* kind of hashing */
                    unsigned int k = last[p];
                    for (j = 0; j < cmap_entries; j++)
                    {
                        int m = (k + j) % cmap_entries;
                        if (v >= cmap[m].min && v < cmap[m].max)
                        {
                            last[p] = m;
                            printf("%c", cmap[m].r);
                            printf("%c", cmap[m].g);
                            printf("%c", cmap[m].b);
                            break;
                        }
                    }
                    if (j == cmap_entries)
                    {
                        fprintf(stderr, "Cannot find colour for %g\n", v);
                        exit(1);
                    }

                }
            }
            else
            {
                /* PPM header */
                printf("P5\n%ld %ld\n65535\n", width, height);
                for (i = 0; i < count; ++i)
                {
                    unsigned long c;
                    unsigned char h, l;
                    double v = values[indices[i]];
                    if (v < lcap)
                    {
                        v = lcap;
                    }
                    if (v > ucap)
                    {
                        v = ucap;
                    }
                    c = ( v - min) * 65535 / (max - min);
                    h = c >> 8;
                    l = c & 0xff;
                    printf("%c", h);
                    printf("%c", l);
                }
            }

            grib_handle_delete(h);
            free(values);
            free(indices);
            break;
        }
        fclose(f);
        if (err)
        {
            fprintf(stderr, "%s\n", grib_get_error_message(err));
            exit(1);
        }
    }
    return 0;
}
