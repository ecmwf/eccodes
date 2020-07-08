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
void grib_vdarray_print(const char* title, const grib_vdarray* source)
{
    size_t i;
    char text[100] = {0,};
    Assert(source);
    printf("%s: vdarray.n=%lu\n", title, (unsigned long)source->n);
    for (i = 0; i < source->n; i++) {
        sprintf(text, " vdarray->v[%lu]", (unsigned long)i);
        //grib_darray_print(text, source->v[i]);
        if( i<= (DYN_DEFAULT_VDARRAY_SIZE_INIT - 1)) {
        	grib_darray_print(text, source->stA[i]);
        } else {
        	grib_darray_print(text, source->dynA[(i-(DYN_DEFAULT_VDARRAY_SIZE_INIT))]);
        }
    }
    printf("\n");
}

grib_vdarray* grib_vdarray_new(grib_context* c, size_t size, size_t incsize)
{
	grib_vdarray* result = NULL;

	if (!c)
		c = grib_context_get_default();

	result = (grib_vdarray*)grib_context_malloc_clear(c, sizeof(grib_vdarray));
	if (!result) {
		grib_context_log(c, GRIB_LOG_ERROR,
				"grib_vdarray_new unable to allocate %d bytes\n", sizeof(grib_vdarray));
		return NULL;
	}

	if(size > DYN_DEFAULT_VDARRAY_SIZE_INIT) {

		result->dynA       = (grib_darray**)grib_context_malloc_clear(c, sizeof(grib_darray*) *  (size-DYN_DEFAULT_VDARRAY_SIZE_INIT)  );

		if (!result->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_vdarray_new unable to allocate %d bytes\n", sizeof(grib_darray*) * (size-DYN_DEFAULT_VDARRAY_SIZE_INIT)  );
			return NULL;
		}
		result->size    = size;

		if (incsize > 0)
			result->incsize             = incsize;
		else
			result->incsize             = DYN_DEFAULT_VDARRAY_SIZE_INCR;

	} else {

		result->dynA					= NULL;
		result->size                = DYN_DEFAULT_VDARRAY_SIZE_INIT;
		result->incsize             = DYN_DEFAULT_VDARRAY_SIZE_INCR;

	}

	result->n       = 0;

	return result;
}

grib_vdarray* grib_vdarray_resize(grib_context* c, grib_vdarray* origin)
{
	int newsize;

	if (origin == NULL)
	{
		return origin;
	}
	if (!c)
		c = grib_context_get_default();

	newsize = origin->incsize + origin->size;

	if (origin->dynA != NULL){

		origin->dynA    = (grib_darray**)grib_context_realloc(c, origin->dynA,  sizeof(grib_darray*) * (newsize-DYN_DEFAULT_VDARRAY_SIZE_INIT) );

		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_vdarray_resize unable to allocate %d bytes\n", sizeof(grib_darray*) * (newsize-DYN_DEFAULT_VDARRAY_SIZE_INIT) );
			return NULL;
		}

	}
	else {

		origin->dynA      = (grib_darray**)grib_context_malloc(c, sizeof(grib_darray*) * (newsize-DYN_DEFAULT_VDARRAY_SIZE_INIT) );

		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_vdarray_resize unable to allocate %d bytes\n", sizeof(grib_darray*) * (newsize-DYN_DEFAULT_VDARRAY_SIZE_INIT ) );
			return NULL;
		}

	}

	origin->size = newsize;

    return origin;
}

grib_vdarray* grib_vdarray_push(grib_context* c, grib_vdarray* source, grib_darray* val)
{

	size_t start_size    = DYN_DEFAULT_VDARRAY_SIZE_INIT;
	size_t start_incsize = DYN_DEFAULT_VDARRAY_SIZE_INCR;

	if (!source)
		source = grib_vdarray_new(c, start_size, start_incsize);

	//If the actual used size of the target is equal to the allowed size, resize the array
	if (source->n == source->size) {

		source = grib_vdarray_resize(c, source);

		//Check if REALLOC WAS POSSIBLE, otherwise it is not possible to insert a new value!
		if (source == NULL)
		{
			return source;
		}

		//insertion in new allocated dynamic array
		source->dynA[ (source->n-(DYN_DEFAULT_VDARRAY_SIZE_INIT)) ] = val;
		source->n++;
		return source;

	}

	//insertion in static array
	if (source->n <= (DYN_DEFAULT_VDARRAY_SIZE_INIT - 1) )
	{
		source->stA[source->n] = val;
	}
	else {
		//insertion in dynamic array
		source->dynA[ (source->n-(DYN_DEFAULT_VDARRAY_SIZE_INIT)) ] = val;
	}

	source->n++;
	return source;
}

/**
 * Return the grib_darray* value at index 'index' for grib_vdarray 'source'
 */
grib_darray* grib_vdarray_get(grib_vdarray* source, size_t index)
{
	if (!source)
	    return NULL;
	if (index < 0)
		return NULL;
	if (index < DYN_DEFAULT_VDARRAY_SIZE_INIT) {
		return source->stA[index];
	} else {
		if (source->dynA != NULL) {
			return source->dynA[(index-DYN_DEFAULT_VDARRAY_SIZE_INIT)];
		}
	}

	return NULL;
}

/**
 * Sets the grib_darray* value 'val' at index 'index' for grib_vdarray 'source'.
 * The method overwrites the existing value, if any.
 * The method returns 1 if the writing operation occurred, otherwise 0.
 */
int grib_vdarray_put (grib_vdarray* source, size_t index, grib_darray* val)
{
	if (!source)
		return 0;
	if (index < 0)
		return 0;
	if (index < DYN_DEFAULT_VDARRAY_SIZE_INIT) {
		source->stA[index] = val;

	} else {
		if (source->dynA != NULL) {
			source->dynA[(index-DYN_DEFAULT_VDARRAY_SIZE_INIT)] = val;
		}
	}

	return 1;
}

void grib_vdarray_delete(grib_context* c, grib_vdarray* source)
{
    if (!source)
        return;
    if (!c)
        grib_context_get_default();
    if (source->dynA)
        grib_context_free(c, source->dynA);
    grib_context_free(c, source);
}

void grib_vdarray_delete_content(grib_context* c, grib_vdarray* source)
{
	int i;

	if (!source)
		return;
	/*	Not needed: the 	grib_vdarray_delete is always called already, after 	grib_vdarray_delete_content
	if (!c)
		grib_context_get_default();
	*/

	//If the actual used size of the target is greater than the allowed size, it is a resized array
	if ( (source->n > source->size) && ( source->dynA != NULL ) ) {
		for (i = 0; i < (source->n - DYN_DEFAULT_VDARRAY_SIZE_INIT); i++) {
			/*	Not needed: the 	grib_vdarray_delete is always called already, after 	grib_vdarray_delete_content
			 * grib_vdarray_delete(c, source->dynA[i]);
			*/
			source->dynA[i] = 0;
		}
	}

	for (i = 0; i < (DYN_DEFAULT_VDARRAY_SIZE_INIT); i++) {
		source->stA[i] = 0;
	}

	source->n = 0;
}

size_t grib_vdarray_used_size(grib_vdarray* source)
{
	return source == NULL ? 0 : source->n;
}
