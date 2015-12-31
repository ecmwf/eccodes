/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api.h"
#include "math.h"

static void usage(char* prog) {
	printf("usage: %s file1.grib file2.grib\n",prog);
	printf("Input files must be gaussian reduced global and sub-area.\n");
	printf("Performs consistency check between global and sub-area.\n");

	exit(1);
}

int main(int argc, char* argv[]) {
	grib_handle *h1,*h2;
	int ret=0;
	FILE *f1,*f2;
	char* infile1;
	char* infile2;
	double *v1,*v2,*v,*gv;
	double *lon1,*lon2,*lon,*glon;
	double *lat1,*lat2,*lat,*glat;
	size_t size1,size2,size,gsize;
	double err1,err2,err;
	int i,j;
	grib_context* c;
	grib_iterator *iter1,*iter2;

	c=grib_context_get_default();

	if (argc < 3) usage(argv[0]);

	infile1=argv[1];
	infile2=argv[2];

	f1=fopen(infile1,"r");
	if (!f1) {
		perror(infile1);
		exit(1);
	}

	f2=fopen(infile2,"r");
	if (!f2) {
		perror(infile2);
		exit(1);
	}

	while ((h1=grib_handle_new_from_file(0,f1,&ret))!=NULL) {
		if ((h2=grib_handle_new_from_file(c,f2,&ret))==NULL) {
			printf("unable to create handle from file %s\n",infile2);
			GRIB_CHECK(ret,0);
			exit(1);
		}
		GRIB_CHECK(grib_get_size(h1,"values",&size1),0);
		v1=(double*)malloc(size1*sizeof(double));
		if (!v1) {printf("unable to allocate v1\n");exit(1);}
		lat1=(double*)malloc(size1*sizeof(double));
		if (!lat1) {printf("unable to allocate lat1\n");exit(1);}
		lon1=(double*)malloc(size1*sizeof(double));
		if (!lon1) {printf("unable to allocate lon1\n");exit(1);}
		GRIB_CHECK(grib_get_double(h1,"packingError",&err1),0);

		iter1=grib_iterator_new(h1,0,&ret);
		GRIB_CHECK(ret,0);

		GRIB_CHECK(grib_get_size(h2,"values",&size2),0);
		v2=(double*)malloc(size2*sizeof(double));
		if (!v2) {printf("unable to allocate v2\n");exit(1);}
		lat2=(double*)malloc(size2*sizeof(double));
		if (!lat2) {printf("unable to allocate lat2\n");exit(1);}
		lon2=(double*)malloc(size2*sizeof(double));
		if (!lon2) {printf("unable to allocate lon2\n");exit(1);}
		GRIB_CHECK(grib_get_double(h2,"packingError",&err2),0);

		iter2=grib_iterator_new(h2,0,&ret);
		GRIB_CHECK(ret,0);

		lat=lat1; lon=lon1; v=v1;
		while(grib_iterator_next(iter1,lat,lon,v)) {
				lat++;
				if (*lon < 0 ) *lon+=360;
				lon++;
				v++;
			}
		lat=lat2; lon=lon2; v=v2;
		while(grib_iterator_next(iter2,lat,lon,v)) {
				lat++;
				if (*lon < 0 ) *lon+=360;
				lon++;
				v++;
			}

		if (size1 > size2) {
			lat=lat2;lon=lon2;v=v2;
			size=size2;
			glat=lat1;glon=lon1;gv=v1;
			gsize=size1;
		} else {
			lat=lat1;lon=lon1;v=v1;
			size=size1;
			glat=lat2;glon=lon2;gv=v2;
			gsize=size2;
		}
		if (err1>err2) err=err1;
		else err=err2;

		j=0;
		for (i=0;i<size;i++) {
			while (j < gsize && ( lat[i]!=glat[j] || lon[i]!=glon[j] ) ) j++;
			if (j == gsize) {
				j=0;
				while (j < gsize && ( lat[i]!=glat[j] || lon[i]!=glon[j] ) ) j++;
			}
			if (j==gsize) {
				printf("lat=%g lon=%g not found in global\n",lat[i],lon[i]);
				exit(1);
			}
			if (fabs(v[i]-gv[j])>err) {
				ret=1;
				printf("lat=%g lon=%g  sub area value=%g global value=%g\n",
					lat[i],lon[i],v[i],gv[j]);
			}

		}
		free(v);free(gv);free(lat);free(glat);free(lon);free(glon);

	}

	fclose(f1);
	fclose(f2);

	return ret;
}
