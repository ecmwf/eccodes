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
 * C Implementation: grib_debug
 *
 * Description: 
 *
 */
 
#include "grib_api_internal.h"
void usage(const char* p)
{
	printf("Usage: %s [-n minimum] [-x maximum] [-b number of bits] [-d decimal scaling] [-2] [--] [values ..]\n",p);
	exit(1);
}

unsigned long pack(double v,double ref,long bscale,long dscale)
{
	double b = grib_power(-bscale,2);
	double d = grib_power( dscale,10);
	return (v*d - ref)*b + 0.5;
}

double unpack(unsigned long v,double ref,long bscale,long dscale)
{
	double b = grib_power( bscale,2);
	double d = grib_power(-dscale,10);
	return (v*b+ref)*d;
}

void try_this(const char* txt,double v,double ref,long bscale,long dscale)
{
	double x;
	if(txt) printf("%-18s  : ",txt);
	x = unpack(pack(v,ref,bscale,dscale),ref,bscale,dscale);
	printf("%g (err=%g)\n",x,v-x); 
}

int main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	double min = 0,max = 100;
	long bits = 16;
	int g2 = 0;
	long dscale = 0;
	double ref;
	double decimal_scale;
	long bscale;
	double dmin,dmax;
	int err=0;

	int c;

	while((c = getopt(argc, argv,"2n:x:b:d:")) != EOF) {
		switch(c) {

			case 'n':
				min = atof(optarg);
				break;

			case 'x':
				max = atof(optarg);
				break;

			case 'b':
				bits = atol(optarg);
				break;

			case 'd':
				dscale = atol(optarg);
				break;

			case '2':
				g2=1;
				break;

			default:
				usage(argv[0]);
				break;
		}
	}

	decimal_scale = grib_power(dscale,10) ;

	dmin = min*decimal_scale;
	dmax = max*decimal_scale;

	if(g2) {
    double* pref=&ref;
    grib_nearest_smaller_ieee_float(dmin,pref);
  }
	else
	  grib_nearest_smaller_ibm_float(dmin,&ref);

	bscale = grib_get_binary_scale_fact(dmax,ref,bits,&err);

	printf("\n");

	printf("Minimum value       : %g\n",min);
	printf("Maximum value       : %g\n",max);
	printf("Bits per values     : %ld\n",bits);
	printf("Binary scale factor : %ld\n",bscale);
	printf("Decimal scale factor: %ld\n",dscale);
	printf("Encoding            : %s\n",g2 ? "IEEE" : "IBM");
	printf("\n");

	if(ref != dmin)
	{
		printf("WARNING: reference value != scaled minimum value\n");
		printf("         reference value = %g\n",ref);
		printf("         scaled minimum  = %g\n",dmin);
		printf("         difference      = %g\n",ref-dmin);
		printf("\n");
	}




	printf("Packing range       : [%0*lx - %0*lx]\n",
		(int)(bits+7)/8*2,pack(min,ref,bscale,dscale),
		(int)(bits+7)/8*2,pack(max,ref,bscale,dscale));

	printf("Bucket size         : %g\n",unpack(1,ref,bscale,dscale) - unpack(0,ref,bscale,dscale));

	printf("\n");
	printf("Packing/unpacking   :\n");
	printf("\n");
	try_this("Minimum",min,ref,bscale,dscale);
	try_this("Maximum",max,ref,bscale,dscale);

	for(;optind < argc; optind++)
		try_this(argv[optind],atof(argv[optind]),ref,bscale,dscale);


	return 0;
}


