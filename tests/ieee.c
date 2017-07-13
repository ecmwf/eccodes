/*
 * Copyright 2005-2017 ECMWF.
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

double min   = -1.0000000001;
double max   = -1.00000000001;
double scale ;

typedef unsigned long (*ieee_to_long_proc) (double);
typedef double        (*long_to_ieee_proc) (unsigned long);

void test(unsigned long input, ieee_to_long_proc ieee_to_long, long_to_ieee_proc long_to_ieee)
{
    double x1 = long_to_ieee(input);
    unsigned long num2 = ieee_to_long(x1);
    printf("\nconv input=%ld to double=%.10f (%g)    and back => %ld\n", input, x1, x1, num2);
    if (num2!=input) printf("ERROR: input=%ld not equal!!!\n", input);
}


#if 0
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
#endif

int main(int argc, char *argv[])
{
#if 1
	unsigned long i = 0;

	test(3242539564, grib_ieee_to_long, grib_long_to_ieee);
	assert(grib_ieee_to_long(grib_long_to_ieee(i)) == i);

	/* The minimum value for which we can convert a long to ieee and back is 0x800000 */
	/* The maximum value for which we can convert a long to ieee and back is 0x7f800000 */
	for(i = 0x800000; i < 0x7f800000; i++)
	{
		/*unsigned long  j = i | 0x80000000;*/

		if(grib_ieee_to_long(grib_long_to_ieee(i)) != i)
		{
			printf("i=%ld i=%lx e=%g x=%lx\n",i,i,grib_long_to_ieee(i),grib_ieee_to_long(grib_long_to_ieee(i)));
			/*assert(grib_ieee_to_long(grib_long_to_ieee(i)) == i);*/
		}
		/*if(grib_ieee_to_long(grib_long_to_ieee(j)) != j)
		{
		    printf("j=%ld i=%lx e=%g x=%lx\n",j,j,grib_long_to_ieee(j),grib_ieee_to_long(grib_long_to_ieee(j)));
		}
		*/
		/*if((i%10000000) == 0)
			printf("i = %08lx(%ld) %08lx(%ld) %g %g\n", i,i,j,j,grib_long_to_ieee(i),grib_long_to_ieee(j));*/
	}

#else

	double ref1 = grib_long_to_ieee(grib_ieee_to_long(min));
	double ref2 = grib_nearest_smaller_ieee_float(min);

	double a   = p(min,ref1);
	double b   = p(min,ref2);
	double c   = p(ref1,ref1);
	double d   = p(ref2,ref2);

    assert(min<max);

#endif
    printf("ALL DONE\n");
	return 0;
}
