/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "wingetopt.h"

#ifdef ECCODES_ON_WINDOWS

#include "string.h"

char *optarg;       /* global argument pointer */
int optind = 0;     /* global argv index */
int opterr = 0;
int getopt(int argc, char *argv[], const char *optstring)
{
    static char *next = 0;
    char c;
    char *cp;
    if (optind == 0)
        next = 0;

    optarg = 0;

    if (next == 0 || *next == '\0')
    {
        if (optind == 0)
            optind++;

        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
        {
            optarg = 0;
            if (optind < argc)
                optarg = argv[optind];
            return -1;
        }

        if (strcmp(argv[optind], "--") == 0)
        {
            optind++;
            optarg = NULL;
            if (optind < argc)
                optarg = argv[optind];
            return  -1;
        }

        next = argv[optind];
        next++;		/* skip past - */
        optind++;
    }

    c = *next++;
    cp = strrchr(optstring, c);

    if (cp == 0 || c == ':')
        return '?';

    cp++;
    if (*cp == ':')
    {
        if (*next != '\0')
        {
            optarg = next;
            next = 0;
        }
        else if (optind < argc)
        {
            optarg = argv[optind];
            optind++;
        }
        else
        {
            return '?';
        }
    }

    return c;
}
#endif

