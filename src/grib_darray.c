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

/* For debugging purposes */
void grib_darray_print(const char* title, const grib_darray* source)
{
    size_t i;
    Assert(source);
    printf("%s: darray.n=%lu  \t", title, (unsigned long)source->n);
    for (i = 0; i < source->n; i++) {
    	if( i<= (DYN_DEFAULT_DARRAY_SIZE_INIT - 1)) {
    		printf("darray[%lu]=%ld\t", (unsigned long)i, source->stA[i]);
    	} else {
    		printf("darray[%lu]=%ld\t", (unsigned long)i, source->dynA[(i-(DYN_DEFAULT_DARRAY_SIZE_INIT))]);
    	}
    }
    printf("\n");
}

grib_darray* grib_darray_new(grib_context* c, size_t size, size_t incsize)
{
    grib_darray* result = NULL;

    if (!c)
        c = grib_context_get_default();

    result = (grib_darray*)grib_context_malloc_clear(c, sizeof(grib_darray));
    if (!result) {
    	grib_context_log(c, GRIB_LOG_ERROR,
    			"grib_darray_new unable to allocate %d bytes\n", sizeof(grib_darray));
    	return NULL;
    }

    if(size > DYN_DEFAULT_DARRAY_SIZE_INIT) {

    	result->dynA       = (double*)grib_context_malloc_clear(c, sizeof(double) * (size-DYN_DEFAULT_DARRAY_SIZE_INIT) );
    	if (!result->dynA) {
    		grib_context_log(c, GRIB_LOG_ERROR,
    				"grib_darray_new unable to allocate %d bytes\n", sizeof(double) * (size-DYN_DEFAULT_DARRAY_SIZE_INIT) );
    		return NULL;
    	}

    	result->size    = size;
    	if (incsize > 0)
    		result->incsize             = incsize;
    	else
    		result->incsize             = DYN_DEFAULT_DARRAY_SIZE_INCR;

    } else {

    	result->dynA					= NULL;
    	result->size                = DYN_DEFAULT_DARRAY_SIZE_INIT;
    	result->incsize             = DYN_DEFAULT_DARRAY_SIZE_INCR;

    }

    result->n       = 0;

    return result;
}

grib_darray* grib_darray_resize(grib_context* c, grib_darray* origin)
{
	int newsize;

	if (origin == NULL)
	{
		return origin;
	}

	newsize = origin->incsize + origin->size;

	if (!c)
		c = grib_context_get_default();

	if (origin->dynA != NULL){

		origin->dynA    = (double*)grib_context_realloc(c, origin->dynA, (newsize-DYN_DEFAULT_DARRAY_SIZE_INIT) * sizeof(double));
		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_darray_resize unable to allocate %d bytes\n", sizeof(double) * (newsize-DYN_DEFAULT_DARRAY_SIZE_INIT) );
			return NULL;
		}

	}
	else {

		origin->dynA = (double*)grib_context_malloc(c, (newsize-DYN_DEFAULT_DARRAY_SIZE_INIT) * sizeof(double));
		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_darray_resize unable to allocate %d bytes\n", sizeof(double) * (newsize-DYN_DEFAULT_DARRAY_SIZE_INIT) );
			return NULL;
		}

	}

	origin->size = newsize;

	return origin;
}

grib_darray* grib_darray_push(grib_context* c, grib_darray* source, double val)
{

    size_t start_size    = DYN_DEFAULT_DARRAY_SIZE_INIT;
    size_t start_incsize = DYN_DEFAULT_DARRAY_SIZE_INCR;

    //If the target is empty, initialize it
    if (!source)
        source = grib_darray_new(c, start_size, start_incsize);

    //If the actual used size of the target is equal to the allowed size, resize the array
    if (source->n == source->size) {

    	source = grib_darray_resize(c, source);

    	//Check if REALLOC WAS POSSIBLE, otherwise it is not possible to insert a new value!
    	if (source == NULL)
    	{
    		return source;
    	}

    	//insertion in new allocated dynamic array
    	source->dynA[ (source->n-(DYN_DEFAULT_DARRAY_SIZE_INIT)) ] = val;
    	source->n++;
    	return source;

    }

    //insertion in static array
    if (source->n <= (DYN_DEFAULT_DARRAY_SIZE_INIT - 1) )
    {
    	source->stA[source->n] = val;
    }
    else {
    	//insertion in dynamic array
    	source->dynA[ (source->n-(DYN_DEFAULT_DARRAY_SIZE_INIT)) ] = val;
    }

    source->n++;
    return source;
}

/**
 * Return the double value at index 'index' for grib_darray 'source'
 */
double grib_darray_get(grib_darray* source, size_t index)
{

	if (!source)
	    return 0;
	if (index < 0)
		return 0;
	if (index < DYN_DEFAULT_DARRAY_SIZE_INIT) {
		return source->stA[index];
	} else {
		if (source->dynA != NULL) {
			return source->dynA[(index-DYN_DEFAULT_DARRAY_SIZE_INIT)];
		}
	}

	return 0;
}

/**
 * The method returns the array currently in use. If the given grib_darray structure pointer is null, it returns NULL.
 */
double* grib_darray_get_arrays_by_reference(grib_darray* source)
{
	if(!source)
			return NULL;

	return source->stA;

	/*
	 * PREVIOUS ATTEMPT
	if (source->n < DYN_DEFAULT_DARRAY_SIZE_INIT) {
		return source->stA;// stA is the static array
	} else {
		if (source->dynA != NULL) {
			return source->dynA; // dynA is the dynamic array
		}
	}
	return NULL;
	*/

}

/**
 * Sets the double value 'val' at index 'index' for grib_darray 'source'.
 * The method overwrites the existing value, if any.
 * The method returns 1 if the writing operation occurred, otherwise 0.
 */
int grib_darray_put (grib_darray* source, size_t index, double val)
{
	if (!source)
		return 0;
	if (index < 0)
		return 0;
	if (index < DYN_DEFAULT_DARRAY_SIZE_INIT) {
		source->stA[index] = val;

	} else {
		if (source->dynA != NULL) {
			source->dynA[(index-DYN_DEFAULT_DARRAY_SIZE_INIT)] = val;
		}
	}

	return 1;
}

void grib_darray_delete(grib_context* c, grib_darray* source)
{
    if (!source)
        return;
    if (!c)
        grib_context_get_default();

    if (source->dynA)
        grib_context_free(c, source->dynA);

    grib_context_free(c, source);
}

/**
 * The method returns an array of double of size source->n containing all the values contained in the 'source' grib_darray, starting to copy from the static array 'sv' and then copying the dynamic array 'v'.
 * Data in the result:
 * 1. From index 0 until index (DYN_DEFAULT_DARRAY_SIZE_INIT -1), data located at the same index in source->sv;
 * 2. From index DYN_DEFAULT_DARRAY_SIZE_INIT until ( source->n ), data located at index (index - DYN_DEFAULT_DARRAY_SIZE_INIT) in source->v;
 */
double* grib_darray_get_array(grib_context* c, grib_darray* source)
{
	double* result;
    size_t i;

    result = (double*)grib_context_malloc_clear(c, sizeof(double) * source->n);

    for (i = 0; i < source->n; i++) {
    	if( i<= (DYN_DEFAULT_DARRAY_SIZE_INIT - 1)) {
    		result[i] = source->stA[i];
    	} else {
    		result[i] = source->dynA[(i-(DYN_DEFAULT_DARRAY_SIZE_INIT))];
    	}
    }

    return result;
}

int grib_darray_is_constant(grib_darray* source, double eps)
{
    int i;
    double val;
    if (source->n == 1)
        return 1;

    val = source->stA[0];

    for (i = 0; i < source->n; i++) {
    	if( i<= (DYN_DEFAULT_DARRAY_SIZE_INIT - 1)) {
    		if (fabs(val - source->stA[i]) > eps)
    			return 0;
    	} else {
    		if (fabs(val - source->dynA[(i-(DYN_DEFAULT_DARRAY_SIZE_INIT))]) > eps)
    		    return 0;
    	}
    }

    return 1;
}

size_t grib_darray_used_size(grib_darray* source)
{
	return source == NULL ? 0 : source->n;
}
