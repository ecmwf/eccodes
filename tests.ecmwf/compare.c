#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "grib_api.h"
#include <sys/param.h>

#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include <math.h>
#include "test.h"

#if 0
double maxRelativeError = 1e-4; /*1e-7;*/
double maxAbsoluteError = 1e-50;
#else
const double maxRelativeError = 1e-7; /*1e-7;*/
const double maxAbsoluteError = 0;
#endif


#define     ISECTION_0  2
#define     ISECTION_1  1024     /* beware of  for ocean data */
#define		ISECTION_2	3000
#define		ISECTION_3	2
#define		ISECTION_4	512

#define		RSECTION_2	512	
#define		RSECTION_3	2
#define		RSECTION_4	1


struct { int code; char *message; } gribex_errors[] = {
	{201 ," An invalid coding/decoding option was requested.",},
	{202 ,"The number of bits per data value exceeds a word length.",},
	{203 ,"Missing data indicated and data field contains non-zero values.",},
	{204 ,"Number of bits per data value is invalid.",},
	{205 ,"Number of bits per data value is same as the number of bits per computer word.",},
	{301 ,"Error in encoding or decoding the letters GRIB.",},
	{302 ,"Error decoding the length of the GRIB message.",},
	{303 ,"Error encoding or decoding the GRIB edition number.",},
	{304 ,"Error decoding octets 22 and 23, experimental edition check.",},
	{305 ,"Input data is not GRIB or pseudo-GRIB.",},
	{401 ,"Error encoding or decoding the length of section 1.",},
	{402 ,"Error encoding or decoding the parameter version number.",},
	{403 ,"Error encoding or decoding six header fields in section 1, from 'identification of centre' to 'indicator of type of level'.",},
	{404 ,"Error encoding or decoding height, pressure, etc of levels.",},
	{405 ,"Error encoding or decoding six fields in section 1, from 'year of century' to 'indicator of unit of time range'.",},
	{406 ,"Error encoding or decoding period of time.",},
	{407 ,"Error encoding or decoding time range indicator.",},
	{408 ,"Error encoding or decoding number averaged.",},
	{409 ,"Error encoding or decoding number missing from averages etc.",},
	{410 ,"Error encoding or decoding century of data or reserved field.",},
	{411 ,"Error encoding or decoding units decimal scale factor.",},
	{412 ,"Error encoding or decoding ECMWF local data.",},
	{413 ,"Grib edition not handled.",},
	{499 ,"Error found when checking values for section 1 against expected valid GRIB values.",},
	{501 ,"Error encoding or decoding length of section 2.",},
	{502 ,"Error encoding or decoding number of vertical coordinate parameters.",},
	{503 ,"Error encoding or decoding location of list of vertical coordinate parameters or list of numbers of points.",},
	{504 ,"Error encoding or decoding data representation type.",},
	{505 ,"Error encoding or decoding number of points along a parallel or meridian.",},
	{506 ,"Error encoding or decoding latitude or longitude of the first grid point.",},
	{507 ,"Error encoding or decoding the components flag.",},
	{508 ,"Error encoding or decoding latitude or longitude of the last grid point.",},
	{509 ,"Error encoding or decoding the i direction increment.",},
	{510 ,"Error encoding or decoding the number of parallels between the pole and the Equator.",},
	{511 ,"Error encoding or decoding scanning mode flags.",},
	{513 ,"Error encoding or decoding the j direction increment.",},
	{514 ,"Error encoding or decoding J,K,M pentagonal resolution parameters.",},
	{515 ,"Error encoding or decoding representation type or mode.",},
	{517 ,"Error encoding or decoding latitude or longitude of southern pole.",},
	{518 ,"Error encoding or decoding angle of rotation.",},
	{519 ,"Error encoding or decoding latitude or longitude of pole of stretching.",},
	{520 ,"Error encoding or decoding stretching factor.",},
	{521 ,"Error encoding or decoding vertical coordinate parameters.",},
	{522 ,"Error encoding or decoding list of numbers of points.",},
	{523 ,"Error encoding or decoding number of points along X or Y axis.",},
	{524 ,"Error encoding or decoding X or Y axis grid lengths.",},
	{525 ,"Error encoding or decoding the projection centre flag.",},
	{526 ,"Error encoding or decoding the latitude or longitude of sub-satellite point.",},
	{527 ,"Error encoding or decoding the diameter of the Earth in X or Y direction.",},
	{528 ,"Error encoding or decoding the X or Y coordinate of the sub-satellite point.",},
	{529 ,"Error encoding or decoding the orientation of the grid or camera angle.",},
	{530 ,"Error encoding or decoding the X or Y coordinate of the origin of sector.",},
	{531 ,"Error inserting/extracting Latin1 or Latin2 of secants points.",},
	{532 ,"Error inserting/extracting latitude/longitude of southern pole.",},
	{540 ,"Error encoding gaussian grid section 2.",},
	{541 ,"Error decoding gaussian grid section 2.",},
	{542 ,"Error encoding lat/long grid section 2.",},
	{543 ,"Error decoding lat/long grid section .",},
	{544 ,"Error encoding spectral section 2.",},
	{545 ,"Error decoding spectral section 2.",},
	{546 ,"Error encoding space view section 2, or error encoding ECMWF ocean section 2.",},
	{547 ,"Error decoding space view section 2, or error decoding ECMWF ocean section 2.",},
	{598 ,"Representation type not handled.",},
	{599 ,"Error found when checking values for section 2 against expected valid GRIB values.",},
	{601 ,"Error encoding or decoding length of section 3.",},
	{602 ,"Error encoding or decoding the number of unused bits at the end of section 3.",},
	{603 ,"Error encoding or decoding the bitmap reference table.",},
	{604 ,"Error encoding or decoding the primary bitmap.",},
	{605 ,"Unable to convert quasi-regular (reduced) gaussian grid with a bitmap.",},
	{699 ,"Error found when checking values for section 3 against expected valid GRIB values.",},
	{701 ,"Error encoding or decoding the length of section 4.",},
	{703 ,"Second-order packing implies additional flags.",},
	{704 ,"Function A or B invalid for second-order packed field (grid-point).",},
	{705 ,"Only simple packing handled for grid data.",},
	{706 ,"Error in decoding the section 4 flag field.",},
	{707 ,"Error encoding or decoding the field scale factor.",},
	{708 ,"Error encoding or decoding the field reference value.",},
	{709 ,"Error encoding or decoding the number of bits per data value.",},
	{710 ,"Output array too small.",},
	{711 ,"Error encoding or decoding a REAL coefficient.",},
	{712 ,"Error encoding or decoding data values.",},
	{713 ,"Error encoding or decoding the 'flag and unused bits' field.",},
	{714 ,"Decoding option is 'X' and number of values is illegal.",},
	{715 ,"Decoding option is 'X' and scanning mode is not north to south and west to east.",},
	{716 ,"Decoding option is 'X' and field is not a gaussian or latitude/longitude grid.",},
	{717 ,"Decoding option is 'X' and a bitmap is encountered.",},
	{720 ,"Error encoding or decoding the octet number at which packed data begins.",},
	{721 ,"Error encoding or decoding the 'extended flag' field.",},
	{722 ,"Error encoding or decoding the first or second dimension of a matrix.",},
	{723 ,"Error encoding or decoding six fields, from first dimension coordinate value onwards.",},
	{724 ,"Error encoding or decoding first or second dimension coefficients.",},
	{725 ,"Error encoding a secondary bitmap.",},
	{726 ,"Output array is not big enough to allow expansion using bitmap(s), or constant (0-bit) field without section 2 not supported.",},
	{727 ,"Invalid predefined bitmap number given, or constant (0-bit) spectral field not supported.",},
	{728 ,"Open of predefined bitmap failed, or constant (0-bit) field function G or B not supported.",},
	{729 ,"Problem reading a predefined bitmap size, or output array is not big enough to allow expansion using bitmaps.",},
	{730 ,"Problem allocating memory for a predefined bitmap.",},
	{731 ,"Problem reading the number of non-missing points in a predefined bitmap.",},
	{732 ,"Problem reading a predefined bitmap.",},
	{733 ,"Problem closing a predefined bitmap file.",},
	{734 ,"Error inserting padding zeroes at end of section 4.",},
	{796 ,"Error extracting pointer to packed data.",},
	{797 ,"Error extracting sub-set truncation or constant (0-bit) field function G or B not supported.",},
	{798 ,"Function is X but no section 2 included.",},
	{799 ,"Error found when checking values for section 4 against expected valid GRIB values.",},
	{800 ,"Error encoding or decoding the 7777 group.",},
	{801 ,"Error inserting or extracting the 7777 group.",},
	{802 ,"Error encoding or decoding the length of the GRIB message.",},
	{805 ,"End of message 7777 group not found.",},
	{806 ,"Error in decoding a primary or secondary bitmap.",},
	{807 ,"Inconsistent values specified for complex packing in KSEC2(6) and KSEC4(4).",},
	{808 ,"Error converting quasi-regular (reduced) gaussian grid to regular.",},
	{809 ,"Error padding unused part of GRIB to zero.",},
	{810 ,"Error inserting dummy zero, or error padding GRIB to multiple of 120.",},
	{811 ,"Unable to handle secondary bitmaps for J option.",},
};

extern double grib_power(long,long);


double bucket_size(grib_handle* g)
{
	long bits;long binary_scale_factor;long decimal_scale_factor;
	unsigned long zero = 0;
	unsigned long one  = 1;
	double z,o,s,d,reference_value;
	int err=0;
  
	err=grib_get_double(g,"referenceValue",&reference_value);
	if (err!=GRIB_SUCCESS) return 0.0;
	
	grib_get_long(g,"binaryScaleFactor",&binary_scale_factor);
	if (err!=GRIB_SUCCESS) return 0.0;
	
	grib_get_long(g,"numberOfBitsContainingEachPackedValue",&bits);
	if (err!=GRIB_SUCCESS) return 0.0;
	
	grib_get_long(g,"decimalScaleFactor",&decimal_scale_factor);
	if (err!=GRIB_SUCCESS) return 0.0;
	
	s = grib_power(binary_scale_factor,2);
	d = grib_power(-decimal_scale_factor,10) ;

	z = (double) (((zero*s)+reference_value)*d);
	o = (double) (((one *s)+reference_value)*d);

	return o-z;
}


static double err(double A, double B)
{
	double relativeError;

	if(fabs(A) <= maxAbsoluteError || fabs(B) <= maxAbsoluteError) 
		relativeError = fabs(A-B);
	else if (fabs(B) > fabs(A))
		relativeError = fabs((A - B) / B);
	else
		relativeError = fabs((A - B) / A);

	return relativeError;
}




#define NUMBER(a) (sizeof(a)/sizeof(a[0]))

extern void grsrnd_(long*);
extern void gribex_(long*, long*, long*, double*, long*, double*, long*, double*,
		long*, char *, long*, long*, const char*, long*,long );

static int cgribex(
		long miss,

		long ksec0[],long   ksec1[], 
		long ksec2[],double rsec2[], 
		long ksec3[],double rsec3[], 
		long ksec4[],double rsec4[], 
		long sec4len,

		char *buffer,
		long *buflen,
		char *op)
{
	long	ret = 1;
	long    in_bufsize  = (*buflen) / sizeof(long);
	long    out_bufsize = in_bufsize;
	long    round = 0;

	rsec3[1] = miss,
	ksec3[1] = miss,

	grsrnd_(&round); /* no rounding !!! */

	gribex_(ksec0, ksec1, ksec2, rsec2, ksec3, rsec3, ksec4, rsec4,
			&sec4len, buffer, &in_bufsize, &out_bufsize, op, &ret,1 );

	*buflen = out_bufsize * sizeof(long);

	return ret;
}

static double proc_cpu()
{
	struct rusage rup;

	if(getrusage(RUSAGE_SELF,&rup) != -1)
	{
		return (rup.ru_utime.tv_sec + rup.ru_utime.tv_usec / 1000000.0 +
				rup.ru_stime.tv_sec + rup.ru_stime.tv_usec / 1000000.0);
	}
	return clock()/(double)CLOCKS_PER_SEC;
}

int compare_values(double* v1,double* v2,int count,const char *what)
{
	int i,j=0;
	double maxError = 0;
	for(i = 0; i < count ;i++)
	{
		double e = err(v1[i],v2[i]);
		if(e > maxError)
		{
			j = i;
			maxError = e;
		}
	}
	fprintf(stdout,"value no %d out of %d %s v1=%g v2=%g abs=%.30e rel=%g\n",j,count,what,v1[j], v2[j],v1[j]-v2[j],maxError);
	if(maxError > maxRelativeError)
	{
		fprintf(stderr,"value no %d out of %d %s v1=%g v2=%g abs=%g rel=%g\n",j,count,what,v1[j], v2[j],v1[j]-v2[j],maxError);
		return 1;
	}
	
	return 0;
}

void get_max_relative_error(double* v1,double* v2,int count,double* maxError,int* id) {
	int i;
	*id=0;
	*maxError = 0;
	for(i = 0; i < count ;i++)	{
		double e = err(v1[i],v2[i]);
		if(e > *maxError)
		{
			*id = i;
			*maxError = e;
		}
	}
}

int compare_with_gribex(grib_handle* g)
{

	long ed = 0;
	int err = 0;
	long missing = 0;
	long is_int;
	long miss;
	int i;
	size_t count;
	long sec4len;
	long len;
	double now,gtime,atime;
	size_t size;
	const void *buffer;
long ksec0[ISECTION_0];
long ksec1[ISECTION_1];
long ksec2[ISECTION_2];
long ksec3[ISECTION_3];
long ksec4[ISECTION_4];

double rsec2[RSECTION_2];
double rsec3[RSECTION_3];
double *rsec4;
double *values;


	if((err = grib_get_long(g,"editionNumber",&ed)))
	{
		fprintf(stderr,"Cannot get editionNumber %d\n",err);
		return err;
	}

	if(ed > 1)
		return 0;


	if((err = grib_get_long(g,"missingValue",&miss)))
	{
		fprintf(stderr,"Cannot get missingValue %d\n",err);
		return err;
	}

	if((err = grib_get_long(g,"integerPointValues",&is_int)))
	{
		fprintf(stderr,"Cannot get integerPointValues %d\n",err);
		return err;
	}

	if((err = grib_get_size(g,"values",&count)))
	{
		fprintf(stderr,"Cannot get number of values %d\n",err);
		return err;
	}

	printf("Number of values %ld\n",(long)count);

	/* printf("number of values is %d\n",count); */

	rsec4  = (double*)malloc(sizeof(double)*(count+100000));
	values = (double*)malloc(sizeof(double)*(count+100000));

	sec4len = count +100000;
	if((err = grib_get_message(g,&buffer,&size))) 
	{
		fprintf(stderr,"Cannot get message %d\n",err);
		goto foo;
	}
	len = size;

	now = proc_cpu();
	err = cgribex(
			miss,
			ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
			ksec4,rsec4,sec4len,
			(char*)buffer,&len,"D");

	gtime = proc_cpu() - now;

	if(err > 0)
	{
		char *e = "unknown";
		for(i = 0; i < NUMBER(gribex_errors); i++)
			if(gribex_errors[i].code == err)
				e = gribex_errors[i].message;

		fprintf(stderr,"GRIBEX error %d: %s\n",err,e);
		fprintf(stderr,"Count api=%ld gribex=%ld\n", (long)count,(long)ksec4[0]);

		goto foo;
	}

	if(ksec4[0] == 0){
		fprintf(stderr,"No GRIB processed - no values in KSEC4\n");
		goto foo;
	}


	now = proc_cpu();
	if((err =  grib_get_double_array(g,"values",values,&count)))
	{
		fprintf(stderr,"Cannot get values %d (%s) count=%ld\n",err,grib_get_error_message(err),(long)count);
		return err;
	}
	atime = proc_cpu() - now;

	if( ksec4[0] < 0)
	{
		missing = 1;
		ksec4[0] = -ksec4[0];
	}

	if(count != ksec4[0])
	{
		fprintf(stderr,"Count mismatch api=%ld gribex=%ld\n",
				(long)count,ksec4[0]);
		err = 1;
		goto foo;
	}

#if 0
	printf("Values: %d gribex: %g api:%g\n",count,gtime,atime);

	if(atime > gtime*1.1)
	{
		fprintf(stderr,"Too slow: gribex: %g api:%g\n",gtime,atime);
		exit(1);
	}
#endif

	if(missing)
	{
		for(i = 0; i < count ;i++)
		{
			if(values[i] != miss)
			{
				fprintf(stderr,"Missing field: value %g not set to %ld\n",values[i],miss);
				err = 1;
				goto foo;

			}
		}
	}
	else if(is_int)
	{
		long long* lval = (long long*)rsec4;
		for(i = 0; i < count ;i++)
			rsec4[i] = lval[i];

		if(compare_values(rsec4,values,count,"gribex"))
		{
			err = 1;
			goto foo;
		}
	}
	else
	{
		if(compare_values(rsec4,values,count,"gribex"))
		{
			err = 1;
			goto foo;
		}

	}

foo:
	free(rsec4);
	free(values);

	return err;
}

int compare_two_gribex(grib_handle* g)
{

	long ed = 0;
	int err = 0;
	long is_int;
	long miss;
	int i;
	size_t count;
	long sec4len;
	long len;
	size_t size;
	const void *buffer;
long ksec0[ISECTION_0];
long ksec1[ISECTION_1];
long ksec2[ISECTION_2];
long ksec3[ISECTION_3];
long ksec4[ISECTION_4];

double rsec2[RSECTION_2];
double rsec3[RSECTION_3];
double *rsec4_1 =0 ;
double *rsec4_2 =0 ;
double *rsec4_3 =0 ;
char   *b;


	if((err = grib_get_long(g,"editionNumber",&ed)))
	{
		fprintf(stderr,"Cannot get editionNumber %d\n",err);
		return err;
	}

	if(ed > 1)
		return 0;


	if((err = grib_get_long(g,"missingValue",&miss)))
	{
		fprintf(stderr,"Cannot get missingValue %d\n",err);
		return err;
	}

	if((err = grib_get_long(g,"integerPointValues",&is_int)))
	{
		fprintf(stderr,"Cannot get integerPointValues %d\n",err);
		return err;
	}

	if(is_int)
		return 0;

	if((err = grib_get_size(g,"values",&count)))
	{
		fprintf(stderr,"Cannot get number of values %d\n",err);
		return err;
	}

	printf("Number of values %ld\n",(long)count);

	/* printf("number of values is %d\n",count); */

	rsec4_1  = (double*)malloc(sizeof(double)*(count+100000));
	rsec4_2 = (double*)malloc(sizeof(double)*(count+100000));
	rsec4_3 = (double*)malloc(sizeof(double)*(count+100000));
	b = (char*)malloc(sizeof(char)*(count+100000));

	sec4len = count +100000;
	if((err = grib_get_message(g,&buffer,&size))) 
	{
		fprintf(stderr,"Cannot get message %d\n",err);
		goto foo;
	}
	len = size;

	err = cgribex(
			miss,
			ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
			ksec4,rsec4_1,sec4len,
			(char*)buffer,&len,"D");


	if(err > 0)
	{
		char *e = "unknown";
		for(i = 0; i < NUMBER(gribex_errors); i++)
			if(gribex_errors[i].code == err)
				e = gribex_errors[i].message;

		fprintf(stderr,"GRIBEX error %d: %s\n",err,e);
		fprintf(stderr,"Count api=%ld gribex=%ld\n", (long)count,ksec4[0]);

		goto foo;
	}

	memcpy(rsec4_3,rsec4_1,sizeof(double)*(count+100000));

	if(ksec4[0] == 0){
		fprintf(stderr,"No GRIB processed - no values in KSEC4\n");
		goto foo;
	}

	if( ksec4[0] < 0)
		goto foo;

	if(count != ksec4[0])
	{
		fprintf(stderr,"Count mismatch api=%ld gribex=%ld\n",
				(long)count,ksec4[0]);
		err = 1;
		goto foo;
	}

	len =  count +100000;
	err = cgribex(
			miss,
			ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
			ksec4,rsec4_1,sec4len,
			(char*)b,&len,"C");


	if(err > 0)
	{
		char *e = "unknown";
		for(i = 0; i < NUMBER(gribex_errors); i++)
			if(gribex_errors[i].code == err)
				e = gribex_errors[i].message;

		fprintf(stderr,"GRIBEX error %d: %s\n",err,e);
		fprintf(stderr,"Count api=%ld gribex=%ld\n", (long)count,ksec4[0]);

		goto foo;
	}

	sec4len = count +100000;
	err = cgribex(
			miss,
			ksec0,ksec1,ksec2,rsec2,ksec3,rsec3,
			ksec4,rsec4_2,sec4len,
			(char*)buffer,&len,"D");


	if(err > 0)
	{
		char *e = "unknown";
		for(i = 0; i < NUMBER(gribex_errors); i++)
			if(gribex_errors[i].code == err)
				e = gribex_errors[i].message;

		fprintf(stderr,"GRIBEX error %d: %s\n",err,e);
		fprintf(stderr,"Count api=%ld gribex=%ld\n", (long)count,ksec4[0]);

		goto foo;
	}

	if(compare_values(rsec4_3,rsec4_2,count,"gribex2"))
	{
		err = 1;
		goto foo;
	}

foo:
	free(rsec4_1);
	free(rsec4_2);
	free(rsec4_3);
	free(b);

	return err;
}

int compare_lists(grib_handle* g,grib_handle *h,const char* what)
{
	int err;
	size_t count = 0;
	size_t ncount = 0;
	double *values1;
	double *values2;

	int e1 = grib_get_size(g,what,&count);
	int e2 = grib_get_size(h,what,&ncount);


	if(e1 == GRIB_NOT_FOUND && e2 == GRIB_NOT_FOUND)
		return 0;

	if(e1)
	{
		fprintf(stderr,"Cannot get number of '%s' %d\n",what,e1);
		return 1;
	}

	if(e2)
	{
		fprintf(stderr,"Cannot get number of '%s' %d\n",what,e2);
		return 1;
	}

	if(ncount != count)
	{
		fprintf(stderr,"'%s' count mismatch %ld -> %ld\n",what,(long)count,(long)ncount);
		return 1;
	}

	values1 = (double*)malloc(sizeof(double)*(count));
	values2 = (double*)malloc(sizeof(double)*(count));


	ncount = count;
	if((err =  grib_get_double_array(g,what,values1,&count)))
	{
		fprintf(stderr,"Cannot get %s1 %d (%s) count=%ld\n",what,err,grib_get_error_message(err),(long)count);
		goto foo;
	}

	if(ncount != count)
	{
		fprintf(stderr,"%s count changes %ld -> %ld\n",what,(long)count,(long)ncount);
		err = 1;
		goto foo;
	}


	ncount = count;
	if((err =  grib_get_double_array(h,what,values2,&count)))
	{
		fprintf(stderr,"Cannot get %s2 %d (%s) count=%ld\n",what,err,grib_get_error_message(err),(long)ncount);
		goto foo;
	}

	if(ncount != count)
	{
		fprintf(stderr,"%s count changes %ld -> %ld\n",what,(long)count,(long)ncount);
		err = 1;
		goto foo;
	}

	err = compare_values(values1,values2,count,what);

foo:
	free(values2);
	free(values1);

	return err;
}

int compare_with_handle(grib_handle* g,grib_handle *h)
{

	double s1,d1,r1,l1;
	double s2,d2,r2,l2;


	l1 = s1 = r1 = d1 = 1e-100;
	grib_get_double(g,"binaryScaleFactor",&s1);
	grib_get_double(g,"referenceValue",&r1);
	grib_get_double(g,"decimalScaleFactor",&d1);
	grib_get_double(g,"laplacianOperator",&l1);


	l2 = s2 = r2 = d2 = 1e-100;
	grib_get_double(h,"binaryScaleFactor",&s2);
	grib_get_double(h,"referenceValue",&r2);
	grib_get_double(h,"decimalScaleFactor",&d2);
	grib_get_double(g,"laplacianOperator",&l2);

	if(s1 != s2)
	{
		fprintf(stderr,
				"Scale factor 1 = %g "
				"Scale factor 2 = %g diff=%g"
				"\n" ,s1,s2,s1-s2);
	}

	if(d1 != d2)
	{
		fprintf(stderr,
				"Decimal Scale factor 1 = %g "
				"Decimal Scale factor 2 = %g diff=%g"
				"\n" ,d1,d2,d1-d2);
	}

	if(r1 != r2)
	{
		fprintf(stderr,
				"Ref. value 1 = %g "
				"Ref. value 2 = %g diff=%g"
				"\n" ,r1,r2,r1-r2);
	}

	if(l1 != l2)
	{
		fprintf(stderr,
				"Laplacian 1 = %g "
				"Laplacian 2 = %g diff=%g"
				"\n" ,l1,l2,l1-l2);
	}



	if(compare_lists(g,h,"values"))
		return 1;

	if(compare_lists(g,h,"pv"))
		return 1;

	if(compare_lists(g,h,"pl"))
		return 1;

	return 0;

}

int save_message(grib_handle* h,const char* path)
{
	FILE* out = fopen(path,"w");
	const void *p;
	size_t len;

	if(!out) {
		perror(path);
		return -1;
	}

	printf("save to %s\n",path);

	grib_get_message(h,&p,&len);

	if(fwrite(p,1,len,out) != len)
	{
		perror(path);
		fclose(out);
		return -1;
	}

	if(fclose(out))
	{
		perror(path);
		return -1;
	}

	return 0;
}


const char* problem(const char* path,const char* name,int field)
{
	static char buf[1024];
	const char *p = path;
	const char *q = path;

		while(*p)
		{
			if(*p == '/') q=p+1;
			p++;
		}

	sprintf(buf,"/vol/dataserv/grib_samples/problems/%s_%s_%d.grib",q,name,field);
	return buf;
}
