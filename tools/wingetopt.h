/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef WINGETOPT_H
#define WINGETOPT_H

/* Microsoft Windows Visual Studio support */
#include "eccodes_windef.h"

#ifdef ECCODES_ON_WINDOWS
/* Microsoft Windows Visual Studio support */
extern int optind, opterr;
extern char *optarg;
int getopt(int argc, char *argv[], const char *optstring);
#endif

#endif

