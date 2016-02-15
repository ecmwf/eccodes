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

#include "grib_api_internal.h"

double min   = -1.0000000001;
double max   = -1.00000000001;
double scale ;

double p(double ref1,double ref2)
{
	double scale = (max-ref1) / 65535;


	unsigned long pack = (unsigned long)(((max - ref1)/scale)+0.5);
	double unpack      = pack*scale + ref2;

	printf(" max: %04X %0.20f %0.20f\n",pack,unpack,max-unpack);

	pack = (unsigned long)(((min- ref1)/scale)+0.5);
	unpack      = pack*scale + ref2;
	printf(" min: %04X %0.20f %0.20f\n",pack,unpack,min-unpack);
	printf("\n");

	return unpack;
}


int main(int argc, char *argv[])
{
#if 1
	unsigned long i = 0;

	assert(grib_ieee_to_long(grib_long_to_ieee(i)) == i);

	for(i = 1; i < 0x7fffffff; i++)
	{
		unsigned long  j = i | 0x80000000;
		if(grib_ieee_to_long(grib_long_to_ieee(i)) != i)
		{
			printf("i=%ld i=%lx e=%g x=%lx\n",i,i,grib_long_to_ieee(i),grib_ieee_to_long(grib_long_to_ieee(i)));
			assert(grib_ieee_to_long(grib_long_to_ieee(i)) == i);
		}
		assert(grib_ieee_to_long(grib_long_to_ieee(j)) == j);
		if((i%100000) == 0)
			printf("i = %08lx %08lx %g %g\n",i,j,grib_long_to_ieee(i),grib_long_to_ieee(j));
	}

#else


	double ref1 = grib_long_to_ieee(grib_ieee_to_long(min));
	double ref2 = grib_nearest_smaller_ieee_float(min);


	double a   = p(min,ref1);
	double b   = p(min,ref2);
	double c   = p(ref1,ref1);
	double d   = p(ref2,ref2);


assert(min<max);


	/*  */


#endif

	return 0;
}
