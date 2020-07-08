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

extern grib_viarrayPOOL viarrayPOOL;

/* For debugging purposes */
void grib_viarray_print(const char* title, const grib_viarray* source)
{
	if(source) {
		size_t i;
		char text[100] = {0,};
		Assert(source);
		printf("%s: viarray.n=%lu\n", title, (unsigned long)source->n);
		for (i = 0; i < source->n; i++) {
			sprintf(text, " viarray->v[%lu]", (unsigned long)i);
			if( i<= (DYN_DEFAULT_VIARRAY_SIZE_INIT - 1)) {
				grib_iarray_print(text, source->stA[i]);
			} else {
				grib_iarray_print(text, source->dynA[(i-(DYN_DEFAULT_VIARRAY_SIZE_INIT))]);
			}
		}
		printf("\n");
	}
}

int grib_viarray_init(grib_viarray* source)
{

	if (!source)
		return 0;

	memset(source->stA,0,sizeof(grib_iarray*)*DYN_DEFAULT_VIARRAY_SIZE_INIT);
	source->size 				= DYN_DEFAULT_VIARRAY_SIZE_INIT;
	source->incsize             = DYN_DEFAULT_VIARRAY_SIZE_INCR;
	source->n					= 0;
	source->dynA				= 0;

	return 1;
}

void grib_viarray_free_dynamic (grib_context* c,  grib_viarray* source)
{
	if (!source)
		return;

	if (source->dynA)
		grib_context_free(c, source->dynA);

}

grib_viarray* grib_viarray_new(grib_context* c, size_t size, size_t incsize)
{
	grib_viarray* result = NULL;

	if (!c)
		c = grib_context_get_default();

	result = (grib_viarray*)grib_context_malloc_clear(c, sizeof(grib_viarray));
	if (!result) {
		grib_context_log(c, GRIB_LOG_ERROR,
				"grib_viarray_new unable to allocate %d bytes\n", sizeof(grib_viarray));
		return NULL;
	}

	if(size > DYN_DEFAULT_VIARRAY_SIZE_INIT) {

		result->dynA       = (grib_iarray**)grib_context_malloc_clear(c, sizeof(grib_iarray*) *  (size-DYN_DEFAULT_VIARRAY_SIZE_INIT)  );

		if (!result->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_viarray_new unable to allocate %d bytes\n", sizeof(grib_iarray*) * (size-DYN_DEFAULT_VIARRAY_SIZE_INIT)  );
			return NULL;
		}
		result->size    = size;

		if (incsize > 0)
			result->incsize             = incsize;
		else
			result->incsize             = DYN_DEFAULT_VIARRAY_SIZE_INCR;

	} else {

		result->dynA					= NULL;
		result->size                = DYN_DEFAULT_VIARRAY_SIZE_INIT;
		result->incsize             = DYN_DEFAULT_VIARRAY_SIZE_INCR;

	}

	result->n       = 0;

	return result;
}

grib_viarray* grib_viarray_resize(grib_context* c, grib_viarray* origin)
{
	int newsize;

	if (!origin)
	{
		return origin;
	}
	if (!c)
		c = grib_context_get_default();

	newsize = origin->incsize + origin->size;

	if (origin->dynA){

		origin->dynA    = (grib_iarray**)grib_context_realloc(c, origin->dynA,  sizeof(grib_iarray*) * (newsize-DYN_DEFAULT_VIARRAY_SIZE_INIT) );

		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_viarray_resize unable to allocate %d bytes\n", sizeof(grib_iarray*) * (newsize-DYN_DEFAULT_VIARRAY_SIZE_INIT) );
			return NULL;
		}

	}
	else {

		origin->dynA      = (grib_iarray**)grib_context_malloc(c, sizeof(grib_iarray*) * (newsize-DYN_DEFAULT_VIARRAY_SIZE_INIT) );

		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_viarray_resize unable to allocate %d bytes\n", sizeof(grib_iarray*) * (newsize-DYN_DEFAULT_VIARRAY_SIZE_INIT ) );
			return NULL;
		}

	}

	origin->size = newsize;

	return origin;
}

void viarrayPoolInit()
{
	int i;

	for (i=0; i<DYN_DEFAULT_VIARRAY_POOL_SIZE; i++) {
		viarrayPOOL.arrayPOOL[i].size=DYN_DEFAULT_VIARRAY_SIZE_INIT;
		viarrayPOOL.arrayPOOL[i].n=0;
		viarrayPOOL.arrayPOOL[i].incsize=DYN_DEFAULT_VIARRAY_SIZE_INCR;
		viarrayPOOL.arrayPOOL[i].dynA=0;
		memset(viarrayPOOL.arrayPOOL[i].stA,0,DYN_DEFAULT_VIARRAY_SIZE_INIT);
	}
}

grib_viarray* grib_viarray_push(grib_context* c, grib_viarray* source, grib_iarray* val)
{
	size_t start_size    = DYN_DEFAULT_VIARRAY_SIZE_INIT;
	size_t start_incsize = DYN_DEFAULT_VIARRAY_SIZE_INCR;

	/*If the target is empty, initialize it*/
	if (!source)
		source = grib_viarray_new(c, start_size, start_incsize);
	/*if (!source) {
		if(viarrayPOOL.poolCounter != (DYN_DEFAULT_VIARRAY_POOL_SIZE - 1) ) {
			source = &(viarrayPOOL.arrayPOOL[viarrayPOOL.poolCounter]);
			viarrayPOOL.poolCounter++;
		} else {
			source = grib_viarray_new(0, start_size, start_incsize);
		}
	}*/

	/*If the actual used size of the target is equal to the allowed size, resize the array*/
	if (source->n == source->size) {

		source = grib_viarray_resize(c, source);

		/*Check if REALLOC WAS POSSIBLE, otherwise it is not possible to insert a new value!*/
		if (!source)
		{
			return source;
		}

		/*insertion in new allocated dynamic array*/
		source->dynA[ (source->n-(DYN_DEFAULT_VIARRAY_SIZE_INIT)) ] = val;
		source->n++;
		return source;

	}

	/*insertion in static array*/
	if (source->n <= (DYN_DEFAULT_VIARRAY_SIZE_INIT - 1) )
	{
		source->stA[source->n] = val;
	}
	else {
		/*insertion in dynamic array*/
		source->dynA[ (source->n-(DYN_DEFAULT_VIARRAY_SIZE_INIT)) ] = val;
	}

	source->n++;
	return source;
}

grib_iarray* grib_viarray_get(const grib_viarray* source, size_t index)
{
	if (!source)
		return NULL;
	if (index < 0)
		return NULL;
	if (index < DYN_DEFAULT_VIARRAY_SIZE_INIT) {
		return source->stA[index];
	} else {
		if (source->dynA) {
			return source->dynA[(index-DYN_DEFAULT_VIARRAY_SIZE_INIT)];
		}
	}

	return NULL;
}

void grib_viarray_delete(grib_context* c, grib_viarray* source)
{
	if (!source)
		return;
	if (!c)
		grib_context_get_default();
	if (source->dynA)
		grib_context_free(c, source->dynA);
	grib_context_free(c, source);
}

void grib_viarray_delete_content(grib_context* c, grib_viarray* source)
{
	int i, statSize;

	if (!source)
		return;

	if(source->n){
		/*If the actual used size of the target is greater than the allowed size, it is a resized array*/
		if ( (source->n > DYN_DEFAULT_VIARRAY_SIZE_INIT) && ( source->dynA) ) {

			for (i =0; i<source->n - DYN_DEFAULT_VIARRAY_SIZE_INIT;i++){
				grib_iarray_delete(source->dynA[i]);
			}
			statSize = DYN_DEFAULT_VIARRAY_SIZE_INIT;
		} else {

			if (source->n == DYN_DEFAULT_VIARRAY_SIZE_INIT){
				statSize = DYN_DEFAULT_VIARRAY_SIZE_INIT;
			}else {
				statSize = source->n;
			}

		}

		for (i =0; i<statSize; i++){
			grib_iarray_delete(source->stA[i]);
		}

		source->n = 0;
	}
}

size_t grib_viarray_used_size(grib_viarray* source)
{
	return (!source) ? 0 : source->n;
}
