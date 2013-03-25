/*
 * C Implementation: keys_iterator
 *
 * Description:
 * Example on how to use keys_iterator functions and the
 * grib_keys_iterator structure to get all the available
 * keys in a message.
 *
 *
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>, (C) 2006
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "grib_api.h"

#define MAX_KEY_LEN  255
#define MAX_VAL_LEN  1024

static void usage(char* progname);

int main(int argc, char *argv[])
{
	/* filter out keys from the key_iterator with
	 * key_iterator_filter_flags  -> GRIB_NO_READ_ONLY_KEYS, GRIB_NO_OPTIONAL_KEYS
	 * set it to 0 if you want all keys
	 */
	   /* unsigned long key_iterator_filter_flags=GRIB_KEYS_ITERATOR_SKIP_READ_ONLY || GRIB_KEYS_ITERATOR_SKIP_OPTIONAL; */
	unsigned long key_iterator_filter_flags=GRIB_KEYS_ITERATOR_ALL_KEYS;

	/* char* namespace=0; */
	char* namespace="mars";

	FILE* f;
	grib_handle* h=NULL;
	grib_keys_iterator* kiter=NULL;
	int err=0;
	int grib_count=0;

	char value[MAX_VAL_LEN];
	size_t vlen=MAX_VAL_LEN;

	if (argc != 2) usage(argv[0]);

	f = fopen(argv[1],"r");
	if(!f) {
		perror(argv[1]);
		exit(1);
	}

	while((h = grib_handle_new_from_file(0,f,&err)) != NULL) {

		grib_count++;
		printf("-- GRIB N. %d --\n",grib_count);
		if(!h) {
			printf("ERROR: Unable to create grib handle\n");
			exit(1);
		}

		kiter=grib_keys_iterator_new(h,key_iterator_filter_flags,namespace);
		if (!kiter) {
			printf("ERROR: Unable to create keys iterator\n");
			exit(1);
		}

		while(grib_keys_iterator_next(kiter))
		{
			const char* name = grib_keys_iterator_get_name(kiter);
			vlen=MAX_VAL_LEN;
			GRIB_CHECK(grib_keys_iterator_get_string(kiter,value,&vlen),name);
			printf("%s = %s\n",name,value);
		}

		grib_keys_iterator_delete(kiter);

	}

	return 0;

}

static void usage(char* progname) {
	printf("\nUsage: %s grib_file\n",progname);
	exit(1);
}

