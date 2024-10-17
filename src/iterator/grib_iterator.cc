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
 *   Jean Baptiste Filippi - 01.11.2005                                    *
 ***************************************************************************/
#include "grib_iterator.h"
#include "../accessor/grib_accessor_class_iterator.h"

namespace eccodes {
namespace grib {
namespace geo {


int Iterator::init(grib_handle* h, grib_arguments* args)
{
    h_ = h;
    return GRIB_SUCCESS;
}

/* For this one, ALL destroy are called */
int Iterator::destroy()
{
    delete context_;
    delete this;
    return GRIB_SUCCESS;
}

eccodes::grib::geo::Iterator* gribIteratorNew(const grib_handle* ch, unsigned long flags, int* error)
{
    grib_handle* h                = (grib_handle*)ch;
    grib_accessor* a              = NULL;
    grib_accessor_iterator_t* ita = NULL;
    *error                        = GRIB_NOT_IMPLEMENTED;
    a                             = grib_find_accessor(h, "ITERATOR");
    ita                           = (grib_accessor_iterator_t*)a;

    if (!a)
        return NULL;

    eccodes::grib::geo::Iterator* iter = grib_iterator_factory(h, ita->args_, flags, error);

    if (iter)
        *error = GRIB_SUCCESS;

    return iter;
}

int gribIteratorDelete(eccodes::grib::geo::Iterator* i)
{
    if (i)
        i->destroy();
    return GRIB_SUCCESS;
}


} // namespace geo
} // namespace grib
} // namespace eccodes


/* 
 * C API Implementation
 * codes_iterator_* wrappers are in eccodes.h and eccodes.cc
 * grib_iterator_* declarations are in grib_api.h
 */

int grib_iterator_reset(grib_iterator* i)
{
    return i->iterator->reset();
}

int grib_iterator_has_next(grib_iterator* i)
{
    return i->iterator->has_next();
}

int grib_iterator_next(grib_iterator* i, double* lat, double* lon, double* value)
{
    return i->iterator->next(lat, lon, value);
}

int grib_iterator_previous(grib_iterator* i, double* lat, double* lon, double* value)
{
    return i->iterator->previous(lat, lon, value);
}

int grib_iterator_destroy(grib_context* c, grib_iterator* i)
{
    return i->iterator->destroy();
}

#if defined(HAVE_GEOGRAPHY)
grib_iterator* grib_iterator_new(const grib_handle* ch, unsigned long flags, int* error)
{
    grib_iterator* i = (grib_iterator*)grib_context_malloc_clear(ch->context, sizeof(grib_iterator));
    i->iterator = eccodes::grib::geo::gribIteratorNew(ch, flags, error);
    if (!i->iterator) {
        grib_context_free(ch->context, i);
        return NULL;
    }
    return i;
}

int grib_iterator_delete(grib_iterator* i)
{
    if (i) { // TODO(maee): check if this is necessary
        grib_context* c = i->iterator->context_; // TODO(maee): find a better solution
        gribIteratorDelete(i->iterator);
        grib_context_free(c, i);
    }
    return 0;
}

#else
grib_iterator* grib_iterator_new(const grib_handle* ch, unsigned long flags, int* error)
{
    *error = GRIB_FUNCTIONALITY_NOT_ENABLED;
    grib_context_log(ch->context, GRIB_LOG_ERROR,
                     "Geoiterator functionality not enabled. Please rebuild with -DENABLE_GEOGRAPHY=ON");

    return NULL;
}

int grib_iterator_delete(grib_iterator* i)
{
    grib_context_log(ch->context, GRIB_LOG_ERROR,
                     "Geoiterator functionality not enabled. Please rebuild with -DENABLE_GEOGRAPHY=ON");
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
#endif
