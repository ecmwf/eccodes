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

#include "eckit/exception/Exceptions.h"

#include "geo/GribSpec.h"


namespace eccodes::geo
{


GeoIterator::GeoIterator(grib_handle* h, unsigned long flags) :
    spec_(new GribSpec(h)), grid_(eckit::geo::GridFactory::build(*spec_)), iter_(grid_->cbegin().release()), end_(grid_->cend().release())
{
    h_          = h;
    class_name_ = "geo_iterator";
    flags_      = flags;
    Assert(h_ != nullptr);

    CODES_CHECK(codes_get_size(h_, "values", &nv_), "");
    Assert(nv_ > 0);

    data_ = (flags_ & GRIB_GEOITERATOR_NO_VALUES) ? nullptr : static_cast<double*>(grib_context_malloc(h_->context, nv_ * sizeof(double)));
    Assert(data_ != nullptr);

    auto size = nv_;
    CODES_CHECK(codes_get_double_array(h_, "values", data_, &size), "");
    Assert(nv_ == size);
}


int GeoIterator::init(grib_handle*, grib_arguments*)
{
    NOTIMP;
}

// The C public API for this does not have a way of returning an error,
// So any exception thrown by eckit will is fatal!
int GeoIterator::next(double* lat, double* lon, double* val) const
{
    if (iter_ == end_) {
        return 0;  // (false)
    }
    try {
        const auto p  = *iter_;
        const auto& q = std::get<eckit::geo::PointLonLat>(p);

        *lat = q.lat;
        *lon = q.lon;
        if (val != nullptr && data_ != nullptr) {
            *val = data_[iter_->index()];
        }

        ++iter_;
    }
    catch(std::exception& e) {
        grib_context_log(h_->context, GRIB_LOG_FATAL, "GeoIterator::next: Exception thrown (%s)", e.what());
        return 0;
    }
    return 1;  // (true)
}


int GeoIterator::previous(double*, double*, double*) const
{
    return GRIB_NOT_IMPLEMENTED;
}


int GeoIterator::reset()
{
    iter_.reset(grid_->cbegin().release());
    return GRIB_SUCCESS;
}


int GeoIterator::destroy()
{
    if (data_ != nullptr) {
        grib_context_free(h_->context, data_);
    }
    return Iterator::destroy();
}


bool GeoIterator::has_next() const
{
    return iter_ != end_;
}


geo_iterator::Iterator*
GeoIterator::create() const
{
    return new GeoIterator{ h_, flags_ };
}


}  // namespace eccodes::geo
