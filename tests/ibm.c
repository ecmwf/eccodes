/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <assert.h>


#include "grib_api_internal.h"

int main(int argc, char *argv[])
{
	unsigned long i = 0;

	assert(grib_ibm_to_long(grib_long_to_ibm(i)) == i);

	for(i = 1; i < 0x7fffffff; i++)
	{
		unsigned long  j = i | 0x80000000;
		if(grib_ibm_to_long(grib_long_to_ibm(i)) != i)
		{
			printf("i=%ld i=%lx e=%g x=%lx\n",i,i,grib_long_to_ibm(i),grib_ibm_to_long(grib_long_to_ibm(i)));
			assert(grib_ibm_to_long(grib_long_to_ibm(i)) == i);
		}
		assert(grib_ibm_to_long(grib_long_to_ibm(j)) == j);
		if((i%100000) == 0)
			printf("i = %08lx %08lx %g %g\n",i,j,grib_long_to_ibm(i),grib_long_to_ibm(j));
	}

	return 0;
}
