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


#include "mir/repres/latlon/ReducedLL.h"

#include <algorithm>
#include <iostream>
#include "atlas/grid.h"
#include "mir/action/misc/AreaCropper.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Compare.h"
#include "mir/util/Domain.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/MD5.h"



namespace mir {
namespace repres {
namespace latlon {


ReducedLL::ReducedLL(const param::MIRParametrisation &parametrisation) :
    Gridded(parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
    ASSERT(parametrisation.get("Nj", Nj_));
    ASSERT(Nj_);
    ASSERT(pl_.size() == Nj_);
}


ReducedLL::~ReducedLL() {
}


void ReducedLL::print(std::ostream &out) const {
    out << "ReducedLL[bbox=" << bbox_ << "]";
}


void ReducedLL::makeName(std::ostream& out) const {
    out << "RLL" << Nj_ << "-";

    eckit::MD5 md5;
    for(auto j = pl_.begin(); j != pl_.end(); ++j) {
        md5 << *j;
    }

    out << std::string(md5);
    bbox_.makeName(out);
}


bool ReducedLL::sameAs(const Representation& other) const {
    const ReducedLL* o = dynamic_cast<const ReducedLL*>(&other);
    return o && (Nj_ == o->Nj_) && (bbox_ == o->bbox_) && (pl_ == o->pl_);
}


void ReducedLL::fill(grib_info &info) const  {
    NOTIMP;
}


void ReducedLL::fill(api::MIRJob &job) const  {
    bbox_.fill(job);
    job.set("pl", pl_);
    job.set("Nj", Nj_);
    NOTIMP;
}


void ReducedLL::cropToDomain(const param::MIRParametrisation &parametrisation, context::Context & ctx) const {
    if (!domain().isGlobal()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(ctx);
    }
}


atlas::Grid ReducedLL::atlasGrid() const {
    const util::Domain dom = domain();

    using atlas::grid::StructuredGrid;
    using atlas::grid::LinearSpacing;
    StructuredGrid::XSpace xspace({ dom.west(), dom.east() }, pl_, !dom.isPeriodicEastWest() );
    StructuredGrid::YSpace yspace( LinearSpacing( { dom.north(), dom.south() }, pl_.size()));

    return atlas::grid::StructuredGrid(xspace, yspace);
}


bool ReducedLL::isPeriodicWestEast() const {
    ASSERT(pl_.size());
    const long maxpl = *std::max_element(pl_.begin(), pl_.end());

    const double GRIB1EPSILON = 0.001;
    eckit::types::CompareApproximatelyEqual<double> cmp(GRIB1EPSILON);

    const Longitude we = bbox_.east() - bbox_.west();
    const Longitude inc = Longitude::GLOBE - we;
    return cmp(inc * maxpl, Longitude::GLOBE);
}


bool ReducedLL::includesNorthPole() const {
    return bbox_.north() == Latitude::NORTH_POLE;
}


bool ReducedLL::includesSouthPole() const {
    return bbox_.north() == Latitude::SOUTH_POLE;
}


void ReducedLL::validate(const std::vector<double> &values) const {
    size_t count = 0;
    for (size_t i = 0; i < pl_.size(); i++) {
        count += pl_[i];
    }
    ASSERT(values.size() == count);
}


class ReducedLLIterator: public Iterator {

    const std::vector<long>& pl_;
    const util::Domain domain_;

    size_t ni_;
    const size_t nj_;

    const eckit::Fraction west_;

    const eckit::Fraction ew_;

    eckit::Fraction inc_west_east_;

    const eckit::Fraction inc_north_south_;

    eckit::Fraction lat_;
    eckit::Fraction lon_;

    size_t i_;
    size_t j_;
    size_t p_;

    size_t count_;
    bool periodic_;

    virtual void print(std::ostream &out) const {
        out << "ReducedLLIterator["
            <<  "domain=" << domain_
            << ",ni="     << ni_
            << ",nj="     << nj_
            << ",i="      << i_
            << ",j="      << j_
            << ",p="      << p_
            << ",count="  << count_
            << "]";
    }

    virtual bool next(Latitude &lat, Longitude &lon) {


        while (j_ < nj_ && i_ < ni_) {

            lat = lat_;
            lon = lon_;

            i_++;
            lon_ += inc_west_east_;


            if (i_ == ni_) {

                j_++;
                lat_ -= inc_north_south_;
                lon_ = west_;

                i_ = 0;

                if (j_ < nj_) {
                    ASSERT(p_ < pl_.size());
                    ni_ = pl_[p_++];
                    ASSERT(ni_ > 1);
                    inc_west_east_ = ew_ / (ni_ - (periodic_? 0:1));
                }

            }

            if (domain_.contains(lat, lon)) {
                count_++;
                return true;
            }
        }
        return false;
    }

public:

    ReducedLLIterator(size_t nj, const std::vector<long>& pl, const util::Domain& dom) :
        pl_(pl),
        domain_(dom),
        nj_(nj),

        west_(domain_.west()),

        ew_(domain_.east() - domain_.west()),

        inc_north_south_(eckit::Fraction(domain_.north() - domain_.south()) / (nj_ - 1)),

        lat_(domain_.north()),
        lon_(west_),
        i_(0),
        j_(0),
        p_(0),
        count_(0),
        periodic_(dom.isPeriodicEastWest()) {

        ni_ = pl_[p_++];
        ASSERT(ni_ > 1);
        inc_west_east_ = ew_ / (ni_ - (periodic_? 0:1));

        // eckit::Log::debug<LibMir>() << *this << std::endl;
    }

};


Iterator *ReducedLL::unrotatedIterator() const {
    return new ReducedLLIterator(Nj_, pl_, domain());
}


Iterator* ReducedLL::rotatedIterator() const {
    return unrotatedIterator();
}


namespace {
static RepresentationBuilder<ReducedLL> reducedLL("reduced_ll"); // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

