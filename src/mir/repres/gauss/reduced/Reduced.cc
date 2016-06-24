/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/repres/gauss/reduced/Reduced.h"

#include <limits>
#include <cmath>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/ScopedPtr.h"

#include "mir/api/MIRJob.h"
#include "mir/log/MIR.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Grib.h"

#include "atlas/grid/Domain.h"


namespace mir {
namespace repres {
namespace reduced {


Reduced::Reduced(size_t N):
    Gaussian(N) {
}


Reduced::Reduced(size_t N, const util::BoundingBox &bbox):
    Gaussian(N, bbox) {
}


Reduced::Reduced(const param::MIRParametrisation &parametrisation):
    Gaussian(parametrisation) {
}


Reduced::~Reduced() {
}


inline void between_0_and_360(double &x) {
    while (x >= 360) {
        x -= 360;
    }
    while (x < 0 ) {
        x += 360;
    }
}


bool Reduced::globalDomain() const {

    // TODO: cache me

    if (bbox_.west() == 0 && bbox_.east() == 360 && bbox_.north() == 90 && bbox_.south() == -90) {
        return true;
    }

    const std::vector<long> &pl = pls();
    ASSERT(pl.size());

    if (N_ * 2 == pl.size()) {

        long most_points = pl[0];
        for (size_t i = 1; i < pl.size(); i++) {
            most_points = std::max(most_points, pl[i]);
        }

        double last = 360.0 - 360.0 / most_points ;
        double ew = bbox_.east() - bbox_.west();

        // FIXME: GRIB=1 is in millidegree, GRIB-2 in in micro-degree
        // Use the precision given by GRIB in this check

        const double epsilon = 1.0 / 1000.0;


        if (ew > last) {
            // The dissemination will put in the GRIB header what is sepecified by the user
            // so, for example if the user specify 359.999999 as the eastern longitude, this
            // value will end up in the header
            return true;
        }

        bool global = fabs(ew - last) < epsilon;
        return global;
    }

    return false;
}


void Reduced::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    const std::vector<long> &pl = pls();

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj = pl.size();
    info.grid.N = N_;

    info.grid.pl = &pl[0];
    info.grid.pl_size = pl.size();


    for (size_t i = 0; i < size_t(info.grid.pl_size); i++) {
        ASSERT(info.grid.pl[i] > 0);
    }

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    bool global = globalDomain();

    size_t j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = global ? 1 : 0;

    if (!global) {
        // It looks like dissemination files have longitudes between 0 and 360
        // See if that logic needs to be moved to BoundingBox

        between_0_and_360(info.grid.longitudeOfFirstGridPointInDegrees);
        between_0_and_360(info.grid.longitudeOfLastGridPointInDegrees);

    }

}


void Reduced::fill(api::MIRJob &job) const  {
    ASSERT(globalDomain());
    job.set("pl", pls());
}


class GaussianIterator: public Iterator {
    std::vector<double> latitudes_;
    const std::vector<long> &pl_;
    util::BoundingBox bbox_;

    size_t ni_;
    size_t nj_;

    size_t i_;
    size_t j_;
    size_t k_;
    size_t p_;

    size_t count_;


    virtual void print(std::ostream &out) const {
        out << "GaussianIterator[]";
    }

    virtual bool next(double &lat, double &lon) {
        while (j_ < nj_ && i_ < ni_) {
            ASSERT(j_ + k_ < latitudes_.size());
            lat = latitudes_[j_ + k_];
            lon = (i_ * 360.0) / ni_;
            i_++;
            if (i_ == ni_) {

                j_++;
                i_ = 0;

                if (j_ < nj_) {
                    ASSERT(p_ < pl_.size());
                    ni_ = pl_[p_++];
                }

            }

            // eckit::Log::trace<MIR>() << "++++++ " << lat << " " << lon << " - " << bbox_ << " -> " << bbox_.contains(lat, lon) << std::endl;

            // eckit::Log::trace<MIR>() << "++++++ " << j_ << " " << nj_ << " - " << i_ << " " << ni_ << std::endl;

            if (bbox_.contains(lat, lon)) {
                count_++;
                return true;
            }
        }
        return false;
    }

public:

    // TODO: Consider keeping a reference on the latitudes and bbox, to avoid copying

    GaussianIterator(const std::vector <double> &latitudes, const std::vector<long> &pl, const util::BoundingBox &bbox):
        latitudes_(latitudes),
        pl_(pl),
        bbox_(bbox),
        i_(0),
        j_(0),
        k_(0),
        p_(0),
        count_(0) {

        // lattitude_ covers the whole globe, while pl_ covers only the current bbox_
        ASSERT(pl_.size() <= latitudes_.size());

        // Position to first latitude
        while (k_ < latitudes_.size() && bbox_.north() < latitudes_[k_]) {
            k_++;
        }
        ASSERT(k_ < latitudes_.size());

        ni_ = pl_[p_++];
        nj_ = pl_.size();

        // eckit::Log::trace<MIR>() << "GaussianIterator ni=" << ni_ << " nj=" << nj_ << " j=" << j_ << " " << bbox_ << std::endl;


    }

    ~GaussianIterator() {
        // std::cout << "~GaussianIterator " << count_ << std::endl;
        // ASSERT(count_ == ni_ * nj_);
    }

};

atlas::grid::Domain Reduced::atlasDomain() const {
    return globalDomain()
           ? atlas::grid::Domain::makeGlobal()
           : atlas::grid::Domain(bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east());
}

Iterator *Reduced::unrotatedIterator() const {
    // Use a global bounding box if global domain, to avoid rounding issues
    // due to GRIB (in)accuracies
    return new GaussianIterator(latitudes(), pls(), globalDomain() ? util::BoundingBox() : bbox_);
}


Iterator* Reduced::rotatedIterator() const {
    return unrotatedIterator();
}


size_t Reduced::frame(std::vector<double> &values, size_t size, double missingValue) const {

    validate(values);

    size_t count = 0;

    // TODO: Check if that logic cannot also be used for other grid, and therefor move it to a higher class

    std::map<size_t, size_t> shape;

    // Iterator is 'unrotated'
    eckit::ScopedPtr<Iterator> iter(unrotatedIterator());

    double prev_lat = std::numeric_limits<double>::max();
    double prev_lon = -std::numeric_limits<double>::max();

    double lat;
    double lon;

    size_t rows = 0;

    size_t dummy = 0; // Used to keep static analyser quiet
    size_t *col = &dummy;

    // Collect the 'shape' of the gaussian field
    // This could be done with the latitudes() and pls(), maybe more efficeintly
    // but this code could also be used for all grids
    // and even be cached (md5 of iterators)

    while (iter->next(lat, lon)) {

        if (lat != prev_lat ) {
            ASSERT(lat < prev_lat); // Assumes scanning mode
            prev_lat = lat;
            prev_lon = -std::numeric_limits<double>::max();

            col = &shape[rows++];
            (*col) = 0;
        }

        ASSERT(lon > prev_lon); // Assumes scanning mode
        prev_lon = lon;
        (*col) ++;
    }

    size_t k = 0;
    for (size_t j = 0; j < rows; j++) {
        size_t cols = shape[j];
        for (size_t i = 0; i < cols; i++) {
            if ( !((i < size) || (j < size) || (i >= cols - size) || (j >= rows - size))) {
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;

}


void Reduced::validate(const std::vector<double> &values) const {

    if (globalDomain()) {
        const std::vector<long> &pl = pls();
        size_t count = 0;
        for (size_t i = 0; i < pl.size(); i++) {
            count += pl[i];
        }
        ASSERT(values.size() == count);
    } else {
        eckit::ScopedPtr<Iterator> it(unrotatedIterator());
        double lat;
        double lon;

        size_t count = 0;
        while (it->next(lat, lon)) {
            if (bbox_.contains(lat, lon)) {
                count++;
            }
        }

        if (values.size() != count) {
            std::ostringstream oss;
            oss << "Failed to validate " << *this << " " << values.size() << " != " << count;
            throw eckit::SeriousBug(oss.str());
        }

        ASSERT(values.size() == count);
    }
}


const Reduced *Reduced::cropped(const util::BoundingBox &bbox) const  {
    const std::vector<long> &pl = pls();
    std::vector<long> newpl;
    newpl.reserve(pl.size());

    const std::vector<double> &lats = latitudes();
    double north = bbox.north();
    double south = bbox.south();

    ASSERT(lats.size() == pl.size());

    for (size_t i = 0; i < lats.size(); i++) {
        if ((lats[i] >= south) && (lats[i] <= north)) {
            newpl.push_back(pl[i]);
        }
    }

    return cropped(bbox, newpl);
}


const Reduced *Reduced::cropped(const util::BoundingBox &bbox, const std::vector<long> &) const {
    std::ostringstream os;
    os << "Reduced::cropped() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


}  // namespace reduced
}  // namespace repres
}  // namespace mir

