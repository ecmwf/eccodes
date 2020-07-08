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

grib_sarray* grib_sarray_new(grib_context* c, size_t size, size_t incsize)
{
    grib_sarray* result = NULL;

    if (!c)
        c = grib_context_get_default();

    result = (grib_sarray*)grib_context_malloc_clear(c, sizeof(grib_sarray));
    if (!result) {
        grib_context_log(c, GRIB_LOG_ERROR,
                         "grib_sarray_new unable to allocate %d bytes\n", sizeof(grib_sarray));
        return NULL;
    }

    if(size > DYN_DEFAULT_SARRAY_SIZE_INIT) {

    	result->dynA       = (char**)grib_context_malloc_clear(c, sizeof(char *) * (size-DYN_DEFAULT_SARRAY_SIZE_INIT) );
    	if (!result->dynA) {
    		grib_context_log(c, GRIB_LOG_ERROR,
    				"grib_sarray_new unable to allocate %d bytes\n", sizeof(char *) * (size-DYN_DEFAULT_SARRAY_SIZE_INIT) );
    		return NULL;
    	}

    	result->size    = size;
    	if (incsize > 0)
    		result->incsize             = incsize;
    	else
    		result->incsize             = DYN_DEFAULT_SARRAY_SIZE_INCR;

    } else {

    	result->dynA					= NULL;
    	result->size                = DYN_DEFAULT_SARRAY_SIZE_INIT;
    	result->incsize             = DYN_DEFAULT_SARRAY_SIZE_INCR;

    }

    result->n       = 0;

    return result;
}

grib_sarray* grib_sarray_resize(grib_context* c, grib_sarray* origin)
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

		origin->dynA    = (char**)grib_context_realloc(c, origin->dynA, (newsize-DYN_DEFAULT_SARRAY_SIZE_INIT) * sizeof(char*));
		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_sarray_resize unable to allocate %d bytes\n", sizeof(char*) * (newsize-DYN_DEFAULT_SARRAY_SIZE_INIT) );
			return NULL;
		}

	}
	else {

		origin->dynA = (char**)grib_context_malloc(c, (newsize-DYN_DEFAULT_SARRAY_SIZE_INIT) * sizeof(char*));
		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_sarray_resize unable to allocate %d bytes\n", sizeof(char*) * (newsize-DYN_DEFAULT_SARRAY_SIZE_INIT) );
			return NULL;
		}

	}

	origin->size = newsize;

	return origin;
}

grib_sarray* grib_sarray_push(grib_context* c, grib_sarray* source, char* val)
{
    size_t start_size    = DYN_DEFAULT_SARRAY_SIZE_INIT;
    size_t start_incsize = DYN_DEFAULT_SARRAY_SIZE_INCR;

    //If the target is empty, initialize it
    if (!source)
    	source = grib_sarray_new(c, start_size, start_incsize);

    //If the actual used size of the target is equal to the allowed size, resize the array
    if (source->n == source->size) {

    	source = grib_sarray_resize(c, source);

    	//Check if REALLOC WAS POSSIBLE, otherwise it is not possible to insert a new value!
    	if (source == NULL)
    	{
    		return source;
    	}

    	//insertion in new allocated dynamic array
    	source->dynA[ (source->n-(DYN_DEFAULT_SARRAY_SIZE_INIT)) ] = val;
    	source->n++;
    	return source;

    }

    //insertion in static array
    if (source->n <= (DYN_DEFAULT_SARRAY_SIZE_INIT - 1) )
    {
    	source->stA[source->n] = val;
    }
    else {
    	//insertion in dynamic array
    	source->dynA[ (source->n-(DYN_DEFAULT_SARRAY_SIZE_INIT)) ] = val;
    }

    source->n++;
    return source;
}

/**
 * Return the char* value at index 'index' for grib_sarray 'source'
 */
char* grib_sarray_get(grib_sarray* source, size_t index)
{

	if (!source)
	    return NULL;
	if (index < 0)
		return NULL;
	if (index < DYN_DEFAULT_SARRAY_SIZE_INIT) {
		return source->stA[index];
	} else {
		if (source->dynA != NULL) {
			return source->dynA[(index-DYN_DEFAULT_SARRAY_SIZE_INIT)];
		}
	}

	return NULL;
}

/**
 * The method returns the array currently in use. If the given grib_sarray structure pointer is null, it returns NULL.
 */
char** grib_sarray_get_arrays_by_reference(grib_sarray* source)
{
	if(!source)
		return NULL;

	return source->stA;

	/* PREVIOUS ATTEMPT
	if (source->n < DYN_DEFAULT_SARRAY_SIZE_INIT) {
		return source->stA;//sv is the static array
	} else {
		if (source->dynA != NULL) {
			return source->dynA;//v is the dynamic array
		}
	}

	return NULL;
	*/
}

/**
 * Sets the char* value 'val' at index 'index' for grib_sarray 'source'.
 * The method overwrites the existing value, if any.
 * The method returns 1 if the writing operation occurred, otherwise 0.
 */
int grib_sarray_put (grib_sarray* source, size_t index, char* val)
{
	if (!source)
		return 0;
	if (index < 0)
		return 0;
	if (index < DYN_DEFAULT_SARRAY_SIZE_INIT) {
		source->stA[index] = val;

	} else {
		if (source->dynA != NULL) {
			source->dynA[(index-DYN_DEFAULT_SARRAY_SIZE_INIT)] = val;
		}
	}

	return 1;
}

void grib_sarray_delete(grib_context* c, grib_sarray* source)
{
	if (!source)
		return;
	if (!c)
		grib_context_get_default();

	if (source->dynA)
		grib_context_free(c, source->dynA);

	grib_context_free(c, source);
}

void grib_sarray_delete_content(grib_context* c, grib_sarray* source)
{
    int i;

    if (!source)
    	return;

    /* This is not needed: the grib_sarray_delete is already always called after the function grib_sarray_delete_content
    if (!c)
    	grib_context_get_default();
    */

    //If the actual used size of the target is greater than the allowed size, it is a resized array
    if (source->n > source->size && ( source->dynA != NULL ) ) {
    	for (i = 0; i < (source->n - DYN_DEFAULT_SARRAY_SIZE_INIT); i++) {
    		/* This is not needed: the grib_sarray_delete is already always called after the function grib_sarray_delete_content
    		 * grib_sarray_delete(c,source->dynA[i]);
    		 */
    		source->dynA[i] = 0;
    	}
    }

    for (i = 0; i < (DYN_DEFAULT_SARRAY_SIZE_INIT); i++) {
    	source->stA[i] = 0;
    }

    source->n = 0;
}

/**
 * The method returns an array of char* of size source->n containing all the values contained in the 'source' grib_sarray, starting to copy from the static array 'sv' and then copying the dynamic array 'v'.
 * Data in the result:
 * 1. From index 0 until index (DYN_DEFAULT_SARRAY_SIZE_INIT -1), data located at the same index in source->sv;
 * 2. From index DYN_DEFAULT_SARRAY_SIZE_INIT until ( source->n ), data located at index (index - DYN_DEFAULT_SARRAY_SIZE_INIT) in source->v;
 */
char** grib_sarray_get_array(grib_context* c, grib_sarray* source)
{
    char** result;
    size_t i;

    result = (char**)grib_context_malloc_clear(c, sizeof(char*) * source->n);

    for (i = 0; i < source->n; i++) {
    	if( i<= (DYN_DEFAULT_SARRAY_SIZE_INIT - 1)) {
    		result[i] = source->stA[i];
    	} else {
    		result[i] = source->dynA[(i-(DYN_DEFAULT_SARRAY_SIZE_INIT))];
    	}
    }

    return result;
}

size_t grib_sarray_used_size(grib_sarray* source)
{
	return source == NULL ? 0 : source->n;
}
