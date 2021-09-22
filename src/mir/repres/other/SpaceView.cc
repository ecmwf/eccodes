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


#include "mir/repres/other/SpaceView.h"

#include <ostream>
#include <sstream>
#include <utility>

#include "eckit/utils/MD5.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Angles.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace other {


static RepresentationBuilder<SpaceView> __builder("space_view");


Point2 SpaceView::Projection::xy(const Point2& /*p*/) const {
    NOTIMP;
}


Point2 SpaceView::Projection::lonlat(const Point2& /*p*/) const {
    NOTIMP;
}


void SpaceView::Projection::hash(eckit::Hash& h) const {
    h << "SpaceView::Projection";
}


SpaceView::SpaceView(const param::MIRParametrisation& param) {
    param.get("earthMajorAxis", earthMajorAxis_ = util::Earth::radius());
    param.get("earthMinorAxis", earthMinorAxis_ = util::Earth::radius());

    // TODO projection_
    double rpol = earthMinorAxis_;
    double req  = earthMajorAxis_;
}


SpaceView::~SpaceView() = default;


bool SpaceView::sameAs(const Representation& other) const {
    auto name = [](const SpaceView& repres) {
        std::ostringstream str;
        repres.makeName(str);
        return str.str();
    };

    auto o = dynamic_cast<const SpaceView*>(&other);
    return (o != nullptr) && name(*this) == name(*o);
}


void SpaceView::fill(grib_info& /*info*/) const {
    NOTIMP;
}


void SpaceView::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
}


void SpaceView::reorder(long scanningMode, MIRValuesVector& values) const {
    grib_reorder(values, scanningMode, y_.size(), x_.size());
}


void SpaceView::validate(const MIRValuesVector& values) const {
    auto count = numberOfPoints();

    Log::debug() << "SpaceView::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << " (" << domain() << ")." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("SpaceView", values.size(), count);
}


void SpaceView::makeName(std::ostream& out) const {
    eckit::MD5 h;
    h << earthMajorAxis_;
    h << earthMinorAxis_;
    h << grid_;
    h << projection_;

    auto type = grid_.projection().spec().getString("type");
    out << "SpaceView-" << (type.empty() ? "" : type + "-") << h.digest();
}


void SpaceView::print(std::ostream& out) const {
    out << "SpaceView[]";
}


Iterator* SpaceView::iterator() const {
    class SpaceViewIterator : public Iterator {
        Projection projection_;
        const LinearSpacing& x_;
        const LinearSpacing& y_;
        PointLonLat pLonLat_;

        size_t ni_;
        size_t nj_;
        size_t i_;
        size_t j_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "SpaceViewIterator[";
            Iterator::print(out);
            out << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) override {
            if (j_ < nj_ && i_ < ni_) {
                pLonLat_ = projection_.lonlat({x_[i_], y_[j_]});
                _lat     = lat(pLonLat_.lat());
                _lon     = lon(pLonLat_.lon());

                if (++i_ == ni_) {
                    i_ = 0;
                    j_++;
                }

                count_++;
                return true;
            }
            return false;
        }

    public:
        SpaceViewIterator(Projection projection, const LinearSpacing& x, const LinearSpacing& y) :
            projection_(std::move(projection)), x_(x), y_(y), ni_(x.size()), nj_(y.size()), i_(0), j_(0), count_(0) {}
        SpaceViewIterator(const SpaceViewIterator&) = delete;
        SpaceViewIterator& operator=(const SpaceViewIterator&) = delete;
    };

    return new SpaceViewIterator(projection_, x_, y_);
}


size_t SpaceView::numberOfPoints() const {
    return x_.size() * y_.size();
}


util::Domain SpaceView::domain() const {
    return {bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east()};
}


const util::BoundingBox& SpaceView::boundingBox() const {
    return bbox_;
}


bool SpaceView::getLongestElementDiagonal(double&) const {
    return false;
}


void SpaceView::estimate(api::MIREstimation&) const {
    NOTIMP;
}


}  // namespace other
}  // namespace repres
}  // namespace mir
