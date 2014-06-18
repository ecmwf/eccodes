/*
 * Copyright 2005-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api.h"

int main(int argc, char* argv[])
{
	FILE* f;
	int err=0;
	char infile[]="../../data/reduced_gaussian_model_level.grib1";
	grib_handle *h=NULL;
	grib_context* c=grib_context_get_default();
	grib_values values[2];
	int nvalues=2;
	int i;
	char* name = NULL;

	f=fopen(infile,"r");
	if (!f) {
		perror(infile);
		exit(1);
	}

	h=grib_handle_new_from_file(c,f,&err);
	if (!h) {
		printf("unable to create handle from file %s\n",infile);
		exit(err);
	}
	fclose(f);

	values[0].type=GRIB_TYPE_LONG;
	values[0].name="centre";
	values[0].long_value=98;

	values[1].type=GRIB_TYPE_LONG;
	values[1].name="level";
	values[1].long_value=2;

	/*GRIB_VALUE_DIFFERENT -> value is different*/
	err=grib_values_check(h,values,nvalues);
	if (err) {
		for (i=0;i<nvalues;i++) {
			if (values[i].error==err) name=(char*)values[i].name;
		}
		printf("ERROR: \"%s\" %s\n",name,grib_get_error_message(err));
	}

	values[1].name="levelll";
	err=grib_values_check(h,values,nvalues);
	if (err) {
		for (i=0;i<nvalues;i++) {
			if (values[i].error==err) name=(char*)values[i].name;
		}
		printf("ERROR: \"%s\" %s\n",name,grib_get_error_message(err));
	}

	return 0;

}
