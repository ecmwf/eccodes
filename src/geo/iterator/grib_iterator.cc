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

#include "eccodes_config.h"

#include "grib_iterator.h"
#include "grib_iterator_factory.h"
#include "accessor/grib_accessor_class_iterator.h"

namespace eccodes::geo_iterator
{

Iterator::Iterator(grib_handle* h, grib_arguments*, unsigned long flags, int& err) :
    h_(h), data_(nullptr), e_(0), nv_(0)
{
    DEBUG_ASSERT(h_);
    err = GRIB_SUCCESS;
}

Iterator::~Iterator() = default;

Iterator* gribIteratorNew(const grib_handle* ch, unsigned long flags, int* error)
{
    auto* a = dynamic_cast<grib_accessor_iterator_t*>(grib_find_accessor(ch, "ITERATOR"));
    if (a != nullptr) {
        *error     = GRIB_SUCCESS;
        auto* iter = grib_iterator_factory(const_cast<grib_handle*>(ch), a->args_, flags, error);
        if (iter != nullptr) {
            return iter;
        }
    }

    *error = GRIB_NOT_IMPLEMENTED;
    return nullptr;
}

int gribIteratorDelete(Iterator* i)
{
    if (i != nullptr) {
        delete i;
        i = nullptr;
    }
    return GRIB_SUCCESS;
}

}  // namespace eccodes::geo_iterator


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
    return i->iterator->has_next() ? 1 : 0;
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
    delete i;
    return GRIB_SUCCESS;
}

#if defined(HAVE_GEOGRAPHY)
grib_iterator* grib_iterator_new(const grib_handle* ch, unsigned long flags, int* error)
{
    auto* i     = static_cast<grib_iterator*>(grib_context_malloc_clear(ch->context, sizeof(grib_iterator)));
    i->iterator = eccodes::geo_iterator::gribIteratorNew(ch, flags, error);

    if (i->iterator == nullptr) {
        grib_context_free(ch->context, i);
        return nullptr;
    }
    return i;
}

int grib_iterator_delete(grib_iterator* i)
{
    if (i != nullptr) {
        grib_context* c = grib_context_get_default();
        gribIteratorDelete(i->iterator);
        grib_context_free(c, i);
    }
    return GRIB_SUCCESS;
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
    grib_context* c = grib_context_get_default();
    grib_context_log(c, GRIB_LOG_ERROR,
                     "Geoiterator functionality not enabled. Please rebuild with -DENABLE_GEOGRAPHY=ON");
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
#endif
