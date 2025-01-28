/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


#include "geo/GeoIterator.h"

#include "eckit/geo/Exceptions.h"

#include "geo/GribSpec.h"


namespace eccodes::geo_iterator
{


GeoIterator::GeoIterator(grib_handle* h, unsigned long flags) :
    spec_(new eccodes::geo::GribSpec(h)), grid_(eckit::geo::GridFactory::build(*spec_)), iter_(grid_->make_next_iterator()), point_(eckit::geo::PointLonLat{})
{
    h_          = h;
    class_name_ = "geo_iterator";
    flags_      = flags;
    ECCODES_ASSERT(h_ != nullptr);

    CODES_CHECK(codes_get_size(h_, "values", &nv_), "");
    ECCODES_ASSERT(nv_ > 0);

    long numberOfPoints = 0;
    grib_get_long_internal(h, "numberOfPoints", &numberOfPoints);
    ECCODES_ASSERT(static_cast<size_t>(numberOfPoints) == nv_);

    if (flags_ & GRIB_GEOITERATOR_NO_VALUES) {
        data_ = nullptr;
    }
    else {
        data_ = static_cast<double*>(grib_context_malloc(h_->context, nv_ * sizeof(double)));
        ECCODES_ASSERT(data_ != nullptr);
        auto size = nv_;
        CODES_CHECK(codes_get_double_array(h_, "values", data_, &size), "");
    }
}

int GeoIterator::init(grib_handle*, grib_arguments*)
{
    NOTIMP;
}

// The C public API for this does not have a way of returning an error,
// So any exception thrown by eckit is fatal!
int GeoIterator::next(double* lat, double* lon, double* val) const
{
    try {
        if (iter_->next(point_)) {
            const auto& q = std::get<eckit::geo::PointLonLat>(point_);

            *lat = q.lat;
            *lon = q.lon;
            if (val != nullptr && data_ != nullptr) {
                *val = data_[iter_->index()];
            }

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
    iter_.reset(grid_->make_next_iterator());
    return GRIB_SUCCESS;
}


int GeoIterator::destroy()
{
    grib_context_free(h_->context, data_);
    return Iterator::destroy();
}


bool GeoIterator::has_next() const
{
    return iter_->has_next();
}


Iterator* GeoIterator::create() const
{
    return new GeoIterator{ h_, flags_ };
}


}  // namespace eccodes::geo_iterator
