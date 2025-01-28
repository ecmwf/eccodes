/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


#pragma once

#include <memory>

#include "eckit/geo/Grid.h"

#include "geo/iterator/grib_iterator.h"


namespace eccodes::geo
{

class GeoIterator : public geo_iterator::Iterator
{
public:
    explicit GeoIterator(grib_handle*, unsigned long flags);

private:
    std::unique_ptr<const eckit::geo::Spec> spec_;
    std::unique_ptr<const eckit::geo::Grid> grid_;

    mutable eckit::geo::Grid::NextIterator iter_;
    mutable eckit::geo::Point point_;

    int init(grib_handle*, grib_arguments*) override;
    int next(double* lat, double* lon, double* val) const override;
    int previous(double* lat, double* lon, double* val) const override;
    int reset() override;
    int destroy() override;
    bool has_next() const override;
    geo_iterator::Iterator* create() const override;
};

}  // namespace eccodes::geo
