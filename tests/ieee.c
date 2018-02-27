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
#include <float.h>

#include "grib_api_internal.h"

double min   = -1.0000000001;
double max   = -1.00000000001;
double scale ;

typedef unsigned long (*ieee_to_long_proc) (double);
typedef double        (*long_to_ieee_proc) (unsigned long);

#if 0
static void test(unsigned long input, ieee_to_long_proc ieee_to_long, long_to_ieee_proc long_to_ieee)
{
    double x1 = long_to_ieee(input);
    unsigned long num2 = ieee_to_long(x1);
    printf("\nconv input=%ld to double=%.10f (%g)    and back => %ld\n", input, x1, x1, num2);
    if (num2!=input) printf("ERROR: input=%ld not equal!!!\n", input);
}

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

/* generate a random floating point number from min to max */
static double randfrom(double minimum, double maximum)
{
    double range = (maximum - minimum);
    double div = RAND_MAX / range;
    return minimum + (rand() / div);
}
/* Return 1 on success, 0 on failure */
static int test_doubles(ieee_to_long_proc ieee_to_long, long_to_ieee_proc long_to_ieee)
{
    const double tolerance = 1e-7;
    const double increment = 1;
    const double max_value = 10 * 1000 * 1000;
    const double min_value = -max_value;
    double d = max_value;
    int num_errors = 0;
    int num_trials = 0;
    double max_reldiff = -DBL_MAX;

    while (d > min_value) {
        double start    = randfrom(0.7,1) * d;
        unsigned long a = ieee_to_long(start);
        double end      = long_to_ieee(a);
        num_trials++;
        if (start != 0.0) {
            double reldiff = fabs(end - start)/start;
            if (reldiff > tolerance) {
                printf("Error: %.10f (diff=%.10f)\n", start, reldiff);
                num_errors ++;
            } else {
                //printf("Success: %.10f (diff=%.10f)\n", start, reldiff);
            }
            if (reldiff > max_reldiff) max_reldiff = reldiff;
        }
        d -= increment;
    }
    printf("trials = %d, errors = %d\n", num_trials,num_errors);
    printf("max reldiff = %g\n", max_reldiff);
    return num_errors==0 ? 1 : 0;
}

int main(int argc, char *argv[])
{
#if 1
	unsigned long i = 0;
	printf("Test doubles with grib_ieee_to_long/grib_long_to_ieee...\n");
	assert( test_doubles(grib_ieee_to_long, grib_long_to_ieee)==1 );

	printf("Test doubles with grib_ieee64_to_long/grib_long_to_ieee64...\n");
	assert( test_doubles(grib_ieee64_to_long, grib_long_to_ieee64)==1 );

    printf("Test integers...\n");
	// test(3242539564, grib_ieee_to_long, grib_long_to_ieee); // This fails!
	assert(grib_ieee_to_long(grib_long_to_ieee(i)) == i);

	/* The minimum value for which we can convert a long to ieee and back is 0x800000 */
	/* The maximum value for which we can convert a long to ieee and back is 0x7f800000 */
	for(i = 0x800000; i < 0x7f800000; i++)
	{
		/*unsigned long  j = i | 0x80000000;*/

		if(grib_ieee_to_long(grib_long_to_ieee(i)) != i)
		{
			printf("i=%lu i=%lx e=%g x=%lx\n",i,i,grib_long_to_ieee(i),grib_ieee_to_long(grib_long_to_ieee(i)));
			/*assert(grib_ieee_to_long(grib_long_to_ieee(i)) == i);*/
			assert(0);
		}
		/*if(grib_ieee_to_long(grib_long_to_ieee(j)) != j)
		{
		    printf("j=%ld i=%lx e=%g x=%lx\n",j,j,grib_long_to_ieee(j),grib_ieee_to_long(grib_long_to_ieee(j)));
		}
		if ((i%1000000) == 0) {
			printf("i = %08lx(%ld) %08lx(%ld) %g %g\n", i,i,j,j,grib_long_to_ieee(i),grib_long_to_ieee(j));
		}*/
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
