/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "grib_iterator_class_gen.h"

namespace eccodes::geo_iterator {

class Regular : public Gen
{
public:
    Regular() { class_name_ = "regular"; }
    Iterator* create() const override { return new Regular(); }

    int init(grib_handle*, grib_arguments*) override;
    int next(double*, double*, double*) const override;
    int previous(double*, double*, double*) const override;
    int destroy() override;

protected:
    long Ni_;
    long Nj_;
    long iScansNegatively_;
    long isRotated_;
    double angleOfRotation_;
    double southPoleLat_;
    double southPoleLon_;
    long jPointsAreConsecutive_;
    long disableUnrotate_;
};

}  // namespace eccodes::geo_iterator