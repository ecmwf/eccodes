/*
 * (C) Copyright 1996-2015 ECMWF.
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

#include "mir/repres/reduced/Gaussian.h"

// #include "atlas/Grid.h"
// #include "atlas/grids/grids.h"
// #include "atlas/grids/GaussianLatitudes.h"
// #include "mir/util/Grib.h"
// #include "eckit/log/Timer.h"
#include "mir/param/MIRParametrisation.h"
#include "eckit/exception/Exceptions.h"
#include "mir/repres/Iterator.h"


#include "atlas/grids/GaussianLatitudes.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace reduced {

Gaussian::Gaussian(size_t N):
    N_(N) {
}

Gaussian::Gaussian(size_t N, const util::BoundingBox &bbox):
    N_(N), bbox_(bbox) {
}

Gaussian::Gaussian(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("N", N_));
}

Gaussian::~Gaussian() {
}


void Gaussian::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    const std::vector<long>& pl = pls();

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj = pl.size() * 2;
    info.grid.N = N_;

    info.grid.pl = &pl[0];
    info.grid.pl_size = pl.size();

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    size_t j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = bbox_.global() ? 1 : 0;

}

class GaussianIterator: public Iterator {
    size_t N_;
    std::vector<double> latitudes_;
    const std::vector<long> &pl_;
    // double north_;
    double west_;
    double we_;
    // double ns_;

    size_t ni_;
    size_t nj_;

    size_t i_;
    size_t j_;
    double lat_;
    double lon_;

    size_t count_;
    size_t total_;



    virtual void print(std::ostream &out) const {
        out << "GaussianIterator[]";
    }

    virtual bool next(double &lat, double &lon) {
        if (j_ < nj_) {
            if (i_ < ni_) {
                lat = lat_;
                lon = lon_;
                lon_ += we_;
                i_++;
                if (i_ == ni_) {
                    lon_ = west_;

                    j_++;
                    i_ = 0;

                    if (j_ < nj_) {
                        ni_ = pl_[j_];
                        lat_ = latitudes_[j_];
                        we_ = 360.0 / ni_;
                    }

                }
                count_++;
                return true;
            }
        }
        return false;
    }

  public:
    GaussianIterator(size_t N, const std::vector <double>& latitudes, const std::vector<long> &pl):
        N_(N),
        latitudes_(latitudes),
        pl_(pl),
        west_(0),
        i_(0),
        j_(0),
        count_(0),
        total_(0) {

        ASSERT(pl_.size());
        ASSERT(latitudes_.size() == pl_.size());

        lat_ = latitudes_[0];
        lon_ = west_;

        ni_ = pl_[0];
        we_ = 360.0 / ni_;

        nj_ = pl_.size();

        for (size_t i = 0; i < pl_.size(); i++) {
            total_ += pl_[i];
        }
    }

    ~GaussianIterator() {
        // ASSERT(count_ == ni_ * nj_);
    }

};

Iterator *Gaussian::iterator() const {
    return new GaussianIterator(N_, latitudes(), pls());
}


const std::vector <double>& Gaussian::latitudes() const {
    if (latitudes_.size() == 0) {
        if (bbox_.global()) {
            ASSERT(pls().size() == N_ * 2);
            latitudes_.resize(N_ * 2);
            atlas::grids::gaussian_latitudes_npole_spole(N_, &latitudes_[0]);
        } else {
            std::vector<double> latitudes(N_ * 2);
            atlas::grids::gaussian_latitudes_npole_spole(N_, &latitudes[0]);

            double north = bbox_.north();
            double south = bbox_.south();

            for (size_t i = 0; i < latitudes.size(); i++) {
                if ((latitudes[i] >= south) && (latitudes[i] <= north)) {
                    latitudes_.push_back(latitudes[i]);
                }
            }
        }
    }
    return latitudes_;
}



void Gaussian::validate(const std::vector<double> &values) const {

    if (bbox_.global()) {
        const std::vector<long> &pl = pls();
        size_t count = 0;
        for (size_t i = 0; i < pl.size(); i++) {
            count += pl[i];
        }
        ASSERT(values.size() == count);
    } else {
        std::auto_ptr<Iterator> it(iterator());
        double lat;
        double lon;

        size_t count = 0;
        while (it->next(lat, lon)) {
            if (bbox_.contains(lat, lon)) {
                count++;
            }
        }

        ASSERT(values.size() == count);
    }
}


Gridded *Gaussian::cropped(const util::BoundingBox &bbox) const  {
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

Gaussian *Gaussian::cropped(const util::BoundingBox &bbox, const std::vector<long> &) const {
    eckit::StrStream os;
    os << "Gaussian::cropped() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

} // namespace reduced
}  // namespace repres
}  // namespace mir

