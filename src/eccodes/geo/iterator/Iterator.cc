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

#if defined(HAVE_ECKIT_GEO)
    #include "eckit/geo/Exceptions.h"

    #include "geo/EckitMainInit.h"
    #include "geo/GeoIterator.h"

    // eccodes macros conflict with eckit
    #ifdef ECCODES_ASSERT
        #undef ECCODES_ASSERT
    #endif
#endif

#include "Iterator.h"
#include "grib_iterator_factory.h"
#include "accessor/Iterator.h"
#include "ExceptionHandler.h"

namespace eccodes::geo_iterator
{

int Iterator::init(grib_handle* h, grib_arguments* args)
{
    h_ = h;
    return GRIB_SUCCESS;
}

/* For this one, ALL destroy are called */
int Iterator::destroy()
{
    return GRIB_SUCCESS;
}

eccodes::geo_iterator::Iterator* gribIteratorNew(const grib_handle* ch, unsigned long flags, int* error)
{
    *error = GRIB_NOT_IMPLEMENTED;

    grib_handle* h          = (grib_handle*)ch;
    grib_accessor* a        = grib_find_accessor(h, "ITERATOR");
    accessor::Iterator* ita = (accessor::Iterator*)a;

    if (!a)
        return NULL;

    eccodes::geo_iterator::Iterator* iter = grib_iterator_factory(h, ita->args_, flags, error);

    if (iter)
        *error = GRIB_SUCCESS;

    return iter;
}

int gribIteratorDelete(eccodes::geo_iterator::Iterator* i)
{
    if (i) {
        i->destroy();
        delete i;
    }
    return GRIB_SUCCESS;
}

}  // namespace eccodes::geo_iterator


/*
 * C API Implementation
 * codes_iterator_* wrappers are in eccodes.h and eccodes.cc
 * grib_iterator_* declarations are in grib_api.h
 */

static int grib_iterator_reset_(grib_iterator* i)
{
    return i->iterator->reset();
}

// C-API: Ensure all exceptions are converted to error codes
int grib_iterator_reset(grib_iterator* i)
{
    auto result = eccodes::handleExceptions(grib_iterator_reset_, i);
    return eccodes::getErrorCode(result);
}

static int grib_iterator_has_next_(grib_iterator* i)
{
    return i->iterator->has_next();
}

// C-API: Ensure all exceptions are converted to error codes
int grib_iterator_has_next(grib_iterator* i)
{
    auto result = eccodes::handleExceptions(grib_iterator_has_next_, i);
    return eccodes::getErrorCode(result);
}

static int grib_iterator_next_(grib_iterator* i, double* lat, double* lon, double* value)
{
    return i->iterator->next(lat, lon, value);
}

// C-API: Ensure all exceptions are converted to error codes
int grib_iterator_next(grib_iterator* i, double* lat, double* lon, double* value)
{
    auto result = eccodes::handleExceptions(grib_iterator_next_, i, lat, lon, value);
    return eccodes::getErrorCode(result);
}

static int grib_iterator_previous_(grib_iterator* i, double* lat, double* lon, double* value)
{
    return i->iterator->previous(lat, lon, value);
}

// C-API: Ensure all exceptions are converted to error codes
int grib_iterator_previous(grib_iterator* i, double* lat, double* lon, double* value)
{
    auto result = eccodes::handleExceptions(grib_iterator_previous_, i, lat, lon, value);
    return eccodes::getErrorCode(result);
}

int grib_iterator_destroy(grib_context* c, grib_iterator* i)
{
    int ret = i->iterator->destroy();
    delete i;
    return ret;
}

#if defined(HAVE_GEOGRAPHY)
static grib_iterator* grib_iterator_new_(const grib_handle* ch, unsigned long flags, int* error)
{
    int err = 0;
    {
        long isGridded = -1;
        err = grib_get_long(ch, "isGridded", &isGridded);
        if (!err && !isGridded) {
            *error = GRIB_NOT_IMPLEMENTED;
            return nullptr;
        }
    }

    grib_iterator* i = (grib_iterator*)grib_context_malloc_clear(ch->context, sizeof(grib_iterator));

    #if defined(HAVE_ECKIT_GEO)

//// TODO(maee)
//// This is TEMPORARY! Remove when eckit geo can handle projected grids
////
    char gridType[128] = {0,};
    size_t gtlen = sizeof(gridType);
    err = grib_get_string(ch, "gridType", gridType, &gtlen);
    bool do_process = true;
    if (!err &&
        (STR_EQUAL(gridType, "lambert") ||
        STR_EQUAL(gridType,  "space_view") ||
        STR_EQUAL(gridType,  "mercator") ||
        STR_EQUAL(gridType,  "lambert_azimuthal_equal_area") ||
        STR_EQUAL(gridType,  "polar_stereographic")) )
    {
        do_process = false;
    }

    const int eckit_geo = ch->context->eckit_geo;  // check environment variable
    if (eckit_geo != 0 && do_process) {
        eccodes::geo::eckit_main_init();
        grib_context_log(ch->context, GRIB_LOG_DEBUG, "Geoiterator: using eckit/geo");
        try {
            if (i->iterator = new eccodes::geo_iterator::GeoIterator(const_cast<grib_handle*>(ch), flags);
                i->iterator != nullptr) {
                *error = i->iterator->init(const_cast<grib_handle*>(ch), nullptr);
                if (*error) {
                    grib_context_log(ch->context, GRIB_LOG_ERROR, "Geoiterator: Error instantiating iterator (%s)",
                                     grib_get_error_message(*error));
                    return nullptr;
                }
                return i;
            }
        }
        catch (eckit::geo::exception::GridUnknownError&) {
            *error = GRIB_NOT_IMPLEMENTED;
            return nullptr;
        }
        catch (eckit::geo::Exception& e) {
            grib_context_log(ch->context, GRIB_LOG_ERROR, "grib_iterator_new: geo::Exception thrown (%s)", e.what());
        }
        catch (std::exception& e) {
            grib_context_log(ch->context, GRIB_LOG_ERROR, "grib_iterator_new: Exception thrown (%s)", e.what());
        }

        *error = GRIB_GEOCALCULUS_PROBLEM;
        return nullptr;
    }
    else
    #endif
    {
        i->iterator = eccodes::geo_iterator::gribIteratorNew(ch, flags, error);
    }

    if (!i->iterator) {
        grib_context_free(ch->context, i);
        return NULL;
    }
    return i;
}

// C-API: Ensure all exceptions are converted to error codes
grib_iterator* grib_iterator_new(const grib_handle* ch, unsigned long flags, int* error)
{
    auto result = eccodes::handleExceptions(grib_iterator_new_, ch, flags, error);
    return eccodes::updateErrorAndReturnValue(result, error);
}

static int grib_iterator_delete_(grib_iterator* i)
{
    if (i) {
        grib_context* c = grib_context_get_default();
        gribIteratorDelete(i->iterator);
        grib_context_free(c, i);
    }
    return GRIB_SUCCESS;
}

// C-API: Ensure all exceptions are converted to error codes
int grib_iterator_delete(grib_iterator* i)
{
    auto result = eccodes::handleExceptions(grib_iterator_delete_, i);
    return eccodes::getErrorCode(result);
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
