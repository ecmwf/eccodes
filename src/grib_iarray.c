/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *
 *   Enrico Fucile
 *   Modified for Performance Study by: CS GMBH
 *
 ***************************************************************************/

#include "grib_api_internal.h"

extern grib_iarrayPOOL iarrayPOOL;

/* For debugging purposes */
void grib_iarray_print(const char* title, const grib_iarray* source)
{
	size_t i;
	Assert(source);
	printf("%s: iarray.n=%lu  \t", title, (unsigned long)source->n);
	for (i = 0; i < source->n; i++) {
		if( i<= (DYN_DEFAULT_IARRAY_SIZE_INIT - 1)) {
			printf("iarray[%lu]=%ld\t", (unsigned long)i, source->stA[i]);
		} else {
			printf("iarray[%lu]=%ld\t", (unsigned long)i, source->dynA[(i-(DYN_DEFAULT_IARRAY_SIZE_INIT))]);
		}
	}
	printf("\n");
}

grib_iarray* grib_iarray_new(grib_context* c, size_t size, size_t incsize)
{
	grib_iarray* result = NULL;

	if (!c)
		c = grib_context_get_default();

	result = (grib_iarray*)grib_context_malloc(c, sizeof(grib_iarray));
	if (!result) {
		grib_context_log(c, GRIB_LOG_ERROR,
				"grib_iarray_new unable to allocate %d bytes\n", sizeof(grib_iarray));
		return NULL;
	}
	result->context             = c;

	if(size > DYN_DEFAULT_IARRAY_SIZE_INIT) {

		result->dynA                   = (long*)grib_context_malloc(c, (sizeof(long) * (size-DYN_DEFAULT_IARRAY_SIZE_INIT)) );

		if (!result->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_iarray_new unable to allocate %d bytes\n", (sizeof(long) * (size-DYN_DEFAULT_IARRAY_SIZE_INIT)) );
			return NULL;
		}

		result->size                = size;
		if (incsize > 0)
			result->incsize             = incsize;
		else
			result->incsize             = DYN_DEFAULT_IARRAY_SIZE_INCR;

	} else {

		result->dynA					= NULL;
		result->size                = DYN_DEFAULT_IARRAY_SIZE_INIT;
		result->incsize             = DYN_DEFAULT_IARRAY_SIZE_INCR;

	}

	result->n                   = 0;

	return result;
}

grib_iarray* grib_iarray_resize(grib_iarray* origin)
{
	grib_context* c;
	int newsize;

	if (!origin)
	{
		return origin;
	}

	newsize = (origin->incsize + origin->size);
	c = origin->context;

	if (!c)
		c = grib_context_get_default();

	if (origin->dynA){

		origin->dynA      = (long*)grib_context_realloc(c, origin->dynA, (newsize-DYN_DEFAULT_IARRAY_SIZE_INIT) * sizeof(long));

		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_iarray_resize unable to allocate %d bytes\n", sizeof(long) * newsize);
			return NULL;
		}

	}
	else {

		origin->dynA      = (long*)grib_context_malloc(c,  (newsize-DYN_DEFAULT_IARRAY_SIZE_INIT ) * sizeof(long) );

		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_iarray_resize unable to allocate %d bytes\n", sizeof(long) * newsize);
			return NULL;
		}

	}

	origin->size                = newsize;

	return origin;
}

void iarrayPoolInit(grib_context* c)
{
	int i;
	grib_context* poolContext = c ? c : 0 ;

	for (i=0; i<DYN_DEFAULT_IARRAY_POOL_SIZE; i++) {
		iarrayPOOL.arrayPOOL[i].context=poolContext;
		iarrayPOOL.arrayPOOL[i].size=DYN_DEFAULT_IARRAY_SIZE_INIT;
		iarrayPOOL.arrayPOOL[i].n=0;
		iarrayPOOL.arrayPOOL[i].incsize=DYN_DEFAULT_IARRAY_SIZE_INCR;
		iarrayPOOL.arrayPOOL[i].dynA=0;
		memset(iarrayPOOL.arrayPOOL[i].stA,0,DYN_DEFAULT_IARRAY_SIZE_INIT);
	}
}

grib_iarray* grib_iarray_push(grib_iarray* source, long val)
{
	size_t start_size    = DYN_DEFAULT_IARRAY_SIZE_INIT;
	size_t start_incsize = DYN_DEFAULT_IARRAY_SIZE_INCR;

	/*If the target is empty, initialize it*/
	/*if (!source)
		source = grib_iarray_new(0, start_size, start_incsize);*/
	if (!source) {
		if(iarrayPOOL.poolCounter != (DYN_DEFAULT_IARRAY_POOL_SIZE - 1) ) {
			grib_context* c = grib_context_get_default();
			iarrayPOOL.arrayPOOL[iarrayPOOL.poolCounter].context=c;
			source = &(iarrayPOOL.arrayPOOL[iarrayPOOL.poolCounter]);
			iarrayPOOL.poolCounter++;
		} else {
			source = grib_iarray_new(0, start_size, start_incsize);
		}
	}

	/*If the actual used size of the target is equal to the allowed size, resize the array*/
	if (source->n == source->size) {

		source = grib_iarray_resize(source);

		/*Check if REALLOC WAS POSSIBLE, otherwise it is not possible to insert a new value!*/
		if (!source)
		{
			return source;
		}

		/*insertion in new allocated dynamic array*/
		source->dynA[ (source->n-(DYN_DEFAULT_IARRAY_SIZE_INIT)) ] = val;
		source->n++;
		return source;

	}

	/*insertion in static array*/
	if (source->n <= (DYN_DEFAULT_IARRAY_SIZE_INIT - 1) )
	{
		source->stA[source->n] = val;
	}
	else {
		/*insertion in dynamic array*/
		source->dynA[ (source->n-(DYN_DEFAULT_IARRAY_SIZE_INIT)) ] = val;
	}

	source->n++;
	return source;
}

long grib_iarray_get(const grib_iarray* source, size_t index)
{

	if (!source)
		return 0;
	if (index < 0)
		return 0;
	if (index < DYN_DEFAULT_IARRAY_SIZE_INIT) {
		return source->stA[index];
	} else {
		if (source->dynA) {
			return source->dynA[index-DYN_DEFAULT_IARRAY_SIZE_INIT];
		}
	}

	return 0;
}

/**
 * The method returns the array currently in use. If the given grib_iarray structure pointer is null, it returns NULL.
 */
long* grib_iarray_get_arrays_by_reference(const grib_iarray* source)
{

	if(!source)
		return NULL;

	/*return source->stA;*/

	/* PREVIOUS ATTEMPT */
	if (source->n < DYN_DEFAULT_IARRAY_SIZE_INIT) {
		return source->stA; /* stA is the static array */
	} else {
		if (source->dynA) {
			return source->dynA;/* dynA is the dynamic array */
		}
	}

	return NULL;
}

void grib_iarray_delete(grib_iarray* source)
{
	grib_context* c;

	if (!source)
		return;
	c = source->context;

	if (source->dynA) {
		grib_context_free(c, source->dynA);
	}

	grib_context_free(c, source);
}

long* grib_iarray_get_array(grib_iarray* source)
{
	long* vv;
	size_t i;
	grib_context* c = grib_context_get_default();

	vv = (long*)grib_context_malloc_clear(c, sizeof(long) * source->n);
	for (i = 0; i < source->n; i++) {
		if( i<= (DYN_DEFAULT_IARRAY_SIZE_INIT - 1)) {
			vv[i] = source->stA[i];
		} else {
			vv[i] = source->dynA[(i-(DYN_DEFAULT_IARRAY_SIZE_INIT))];
		}
	}

	return vv;
}

size_t grib_iarray_used_size(grib_iarray* source)
{
	return (!source) ? 0 : source->n;
}
