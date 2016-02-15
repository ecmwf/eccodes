/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "load.h"

char *prog;

void usage(const char *prog)
{
	fprintf(stderr,"%s: [-l] in out\n",prog);
	exit(1);
}

int main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind, opterr, optopt;
	int c;
	int lflag = 0;
	prog = argv[0];

	setbuf(stdout,0);

	while((c = getopt(argc, argv,"l")) != EOF) {
		switch(c) {
			case 'l':
				lflag++;
				break;

			default:
				usage(prog);
				break;
		}
	}

	if(argc-optind < 2) usage(prog);


	if(lflag)
		return load_file(argv[argc-2],argv[argc-1]);
	else
		return dump_file(argv[argc-2],argv[argc-1]);

	return 0;

}

