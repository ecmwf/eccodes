/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

void usage(char* prog) {printf("usage: %s split order interpolated.grib original.grib\n",prog);exit(1);}

int get_box(grib_handle *h,double *east,double *north,double *west,double *south) {
	int ret=0;
	long iScansNegatively,jScansPositively;

	ret=grib_get_long(h,"iScansNegatively",&iScansNegatively);
	if (ret) return ret;
	ret=grib_get_long(h,"jScansPositively",&jScansPositively);
	if (ret) return ret;
	if (iScansNegatively) {
		grib_get_double(h,"longitudeOfFirstGridPointInDegrees",west);
		grib_get_double(h,"longitudeOfLastGridPointInDegrees",east);
	} else {
		grib_get_double(h,"longitudeOfFirstGridPointInDegrees",east);
		grib_get_double(h,"longitudeOfLastGridPointInDegrees",west);
	}
	if (jScansPositively) {
		grib_get_double(h,"latitudeOfFirstGridPointInDegrees",south);
		grib_get_double(h,"latitudeOfLastGridPointInDegrees",north);
	} else {
		grib_get_double(h,"latitudeOfFirstGridPointInDegrees",north);
		grib_get_double(h,"latitudeOfLastGridPointInDegrees",south);
	}
	if (*east>*west) *east-=360;
	return ret;
}

int main(int argc, char* argv[]) {
	int order,count,i,j,k,l;
	FILE *f1,*f2;
	double *m1,*m2,*dm;
	int ret=0;
	long numberOfValues;
	double geast,gwest,gnorth,gsouth;
	double *east,*west,*north,*south;
	grib_handle *h1,*h2;
	grib_context *c=grib_context_get_default();
	int split=1;

	if (argc!=5) usage(argv[0]);

	split=atoi(argv[1]);
	order=atoi(argv[2]);

	f1=fopen(argv[3],"r");
	if (!f1) {
		perror(argv[3]);
		exit(1);
	}

	f2=fopen(argv[4],"r");
	if (!f2) {
		perror(argv[4]);
		exit(1);
	}

	m1=(double*)grib_context_malloc_clear(c,sizeof(double)*order*order);
	m2=(double*)grib_context_malloc_clear(c,sizeof(double)*order*order);
	dm=(double*)grib_context_malloc_clear(c,sizeof(double)*order*order);
	east=(double*)grib_context_malloc_clear(c,sizeof(double)*split);
	south=(double*)grib_context_malloc_clear(c,sizeof(double)*split);
	north=(double*)grib_context_malloc_clear(c,sizeof(double)*split);
	west=(double*)grib_context_malloc_clear(c,sizeof(double)*split);

	count=0;
	while ((h1=grib_handle_new_from_file(0,f1,&ret))!=NULL && (h2=grib_handle_new_from_file(0,f2,&ret))!=NULL) {
		get_box(h1,&geast,&gnorth,&gwest,&gsouth);
		if (split>1) {
			double inc=0;
			double d=(gwest-geast)/split;
			
			inc=0;
			for (i=0;i<split;i++) {
				east[i]=geast+inc;
				inc+=d;
				west[i]=geast+inc;
			}
			d=(gnorth-gsouth)/split;
			inc=0;
			for (i=0;i<split;i++) {
				south[i]=gsouth+inc;
				inc+=d;
				north[i]=gsouth+inc;
			}
		} else {
			east[0]=geast;
			north[0]=gnorth;
			south[0]=gsouth;
			west[0]=gwest;
		}

		for (k=0;k<split;k++) {
			for (l=0;l<split;l++) {
				printf("- %d - east=%.3f \twest=%.3f \tsouth=%.3f \tnorth=%.3f\n",count+1,east[k],west[k],south[l],north[l]);
				grib_moments(h1,east[k],north[l],west[k],south[l],order,m1,&numberOfValues);
				grib_moments(h2,east[k],north[l],west[k],south[l],order,m2,&numberOfValues);

				printf("numberOfValues=%ld\n",numberOfValues);
				for (i=0;i<order;i++) {
					for (j=0;j<order;j++) {
						printf("  (%d,%d)   ",i,j);
					}
				}
				printf("\n");
				for (i=0;i<order*order;i++) printf("% .2e ",m1[i]);
				printf("\n");
				for (i=0;i<order*order;i++) printf("% .2e ",m2[i]);
				printf("\n");
				for (i=0;i<order*order;i++) printf("% .2e ",fabs((m1[i]-m2[i])/m2[i]));
				printf("\n\n");
			}
		}
		grib_handle_delete(h2);
		grib_handle_delete(h1);

		count++;
	}

	grib_context_free(c,m1);
	grib_context_free(c,m2);
	grib_context_free(c,dm);
	grib_context_free(c,east);
	grib_context_free(c,south);
	grib_context_free(c,north);
	grib_context_free(c,west);

	fclose(f1);
	fclose(f2);

	return 0;
}
