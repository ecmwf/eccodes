/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eckit/geo/Exceptions.h"

#include "eccodes/geo/GeoIterator.h"
#include "eccodes/geo/GribToSpec.h"


namespace eccodes::geo_iterator
{


GeoIterator::GeoIterator(grib_handle* h, unsigned long flags) :
    spec_(new eccodes::geo::GribToSpec(h)),
    grid_(eckit::geo::GridFactory::build(*spec_)),
    iter_(grid_->begin())
{
    h_          = h;
    class_name_ = "geo_iterator";
    flags_      = flags;
    ECCODES_ASSERT(h_ != nullptr);
}

int GeoIterator::init(grib_handle* h, grib_arguments*)
{
    ECCODES_ASSERT(h == h_);
    int err = codes_get_size(h_, "values", &nv_);
    if (err) return err;
    if (nv_ == 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Geoiterator: size(values) is 0!");
        return GRIB_WRONG_GRID;
    }

    long numberOfPoints = 0;
    err = grib_get_long_internal(h_, "numberOfPoints", &numberOfPoints);
    if (err) return err;

    if ( (flags_ & GRIB_GEOITERATOR_NO_VALUES) == 0 ) { // Do check the data values count
        if ((size_t)numberOfPoints != nv_) {
            grib_context_log(h_->context, GRIB_LOG_ERROR,
                "Geoiterator: numberOfPoints != size(values) (%ld!=%ld)", numberOfPoints, nv_);
            return GRIB_WRONG_GRID;
        }
    }

    if (flags_ & GRIB_GEOITERATOR_NO_VALUES) {
        data_ = nullptr;
    }
    else {
        data_ = static_cast<double*>(grib_context_malloc(h_->context, nv_ * sizeof(double)));
        if (!data_) return GRIB_OUT_OF_MEMORY;
        auto size = nv_;
        err = codes_get_double_array(h_, "values", data_, &size);
        if (err) return err;
    }
    return GRIB_SUCCESS;
}

// The C public API for this does not have a way of returning an error,
// So any exception thrown by eckit is fatal!
int GeoIterator::next(double* lat, double* lon, double* val) const
{
    try {
        if (iter_) {
            const auto& q = std::get<eckit::geo::PointLonLat>(*iter_);

            *lat = q.lat;
            *lon = q.lon;
            if (val != nullptr && data_ != nullptr) {
                const size_t i = iter_->index();
                if (i < nv_)
                    *val = data_[i];
            }

            ++iter_;
            return 1;  // (true)
        }
    }
    catch (eckit::geo::Exception& e) {
        grib_context_log(h_->context, GRIB_LOG_FATAL, "GeoIterator::next: geo::Exception thrown (%s)", e.what());
    }
    catch (std::exception& e) {
        grib_context_log(h_->context, GRIB_LOG_FATAL, "GeoIterator::next: Exception thrown (%s)", e.what());
    }

    return 0;  // (false)
}

int GeoIterator::previous(double*, double*, double*) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int GeoIterator::reset()
{
    iter_ = grid_->begin();
    return GRIB_SUCCESS;
}

int GeoIterator::destroy()
{
    grib_context_free(h_->context, data_);
    return Iterator::destroy();
}

bool GeoIterator::has_next() const
{
    auto it = iter_;
    return ++it;
}

Iterator* GeoIterator::create() const
{
    return new GeoIterator{ h_, flags_ };
}

}  // namespace eccodes::geo_iterator
