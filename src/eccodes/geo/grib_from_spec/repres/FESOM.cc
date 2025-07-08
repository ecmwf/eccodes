/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


namespace eccodes::geo
{


void FESOM_fillGrib(grib_info& info) const
{
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    info.extra_set("unstructuredGridType", grid_->name().c_str());
    info.extra_set("unstructuredGridSubtype", grid_->arrangement().c_str());
    info.extra_set("uuidOfHGrid", grid_->uid().c_str());
}


}  // namespace eccodes::geo
