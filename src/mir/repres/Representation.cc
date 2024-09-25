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


#include "mir/repres/Representation.h"

#include <memory>
#include <sstream>

#include "mir/data/MIRField.h"
#include "mir/key/grid/Grid.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/UnstructuredGrid.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Mutex.h"


namespace mir::repres {


Representation::Representation() = default;


Representation::~Representation() = default;


RepresentationHandle::RepresentationHandle(const Representation* r) : representation_(r) {
    if (representation_ != nullptr) {
        representation_->attach();
    }
}


RepresentationHandle::RepresentationHandle(const RepresentationHandle& rh) : representation_(rh) {
    if (representation_ != nullptr) {
        representation_->attach();
    }
}


RepresentationHandle::~RepresentationHandle() {
    if (representation_ != nullptr) {
        representation_->detach();
    }
}


std::vector<util::GridBox> Representation::gridBoxes() const {
    std::ostringstream os;
    os << "Representation::gridBoxes() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


void Representation::estimate(api::MIREstimation& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::estimate() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool Representation::isGlobal() const {
    return domain().isGlobal();
}


bool Representation::isPeriodicWestEast() const {
    std::ostringstream os;
    os << "Representation::isPeriodicWestEast() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool Representation::includesNorthPole() const {
    std::ostringstream os;
    os << "Representation::includesNorthPole() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool Representation::includesSouthPole() const {
    std::ostringstream os;
    os << "Representation::includesSouthPole() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


void Representation::validate(const MIRValuesVector& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::validate() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


void Representation::fillGrib(grib_info& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::fillGrib(grib_info&) not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


void Representation::fillJob(api::MIRJob& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::fillJob(api::MIRJob&) not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


void Representation::fillMeshGen(util::MeshGeneratorParameters& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::fillMeshGen(util::MeshGeneratorParameters&) not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


const Representation* Representation::croppedRepresentation(const util::BoundingBox& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::croppedRepresentation() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


const Representation* Representation::truncate(size_t /*unused*/, const MIRValuesVector& /*unused*/,
                                               MIRValuesVector& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::truncate() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


const std::string& Representation::uniqueName() const {
    if (uniqueName_.empty()) {
        std::ostringstream os;
        makeName(os);
        uniqueName_ = os.str();
    }

    ASSERT(!uniqueName_.empty());
    return uniqueName_;
}


void Representation::makeName(std::ostream& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::makeName(std::ostream& out) not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

void Representation::json(eckit::JSON& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::json(eckit::JSON& out) not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool Representation::sameAs(const Representation& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::sameAs() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


atlas::Grid Representation::atlasGrid() const {
    std::ostringstream os;
    os << "Representation::atlasGrid() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


util::Domain Representation::domain() const {
    std::ostringstream os;
    os << "Representation::domain() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


const util::BoundingBox& Representation::boundingBox() const {
    std::ostringstream os;
    os << "Representation::boundingBox() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


util::BoundingBox Representation::extendBoundingBox(const util::BoundingBox& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::extendBoundingBox() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool Representation::extendBoundingBoxOnIntersect() const {
    std::ostringstream os;
    os << "Representation::extendBoundingBoxOnIntersect() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool Representation::crop(util::BoundingBox& /*unused*/, util::IndexMapping& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::crop() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


size_t Representation::truncation() const {
    std::ostringstream os;
    os << "Representation::truncation() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


size_t Representation::numberOfPoints() const {
    std::ostringstream os;
    os << "Representation::numberOfPoints() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


size_t Representation::numberOfValues() const {
    std::ostringstream os;
    os << "Representation::numberOfValues() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool Representation::getLongestElementDiagonal(double& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::getLongestElementDiagonal() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


void Representation::comparison(std::string& /*unused*/) const {
    // do nothing
}


size_t Representation::frame(MIRValuesVector& /*unused*/, size_t /*unused*/, double /*unused*/, bool /*unused*/) const {
    std::ostringstream os;
    os << "Representation::frame() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

std::string Representation::factory() const {
    std::ostringstream os;
    os << "Representation::factory() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

void Representation::reorder(long /*unused*/, MIRValuesVector& /*unused*/) const {
    std::ostringstream os;
    os << "Representation::reorder() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


Iterator* Representation::iterator() const {
    std::ostringstream os;
    os << "Representation::iterator() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


const Representation* Representation::globalise(data::MIRField& field) const {
    const util::Domain dom = domain();

    if (dom.isGlobal()) {
        return nullptr;
    }

    // TODO: cache me

    RepresentationHandle octahedral(key::grid::Grid::lookup("O320").representation());
    size_t size = octahedral->numberOfPoints() + numberOfPoints();

    std::vector<double> latitudes;
    latitudes.resize(size);
    std::vector<double> longitudes;
    longitudes.resize(size);

    std::unique_ptr<Iterator> it(octahedral->iterator());
    while (it->next()) {
        const auto& p = it->pointUnrotated();
        latitudes.push_back(p.lat().value());
        longitudes.push_back(p.lon().value());
    }

    size_t extra = 0;
    while (it->next()) {
        const auto& p = it->pointUnrotated();
        if (!dom.contains(p)) {
            latitudes.push_back(p.lat().value());
            longitudes.push_back(p.lon().value());
            extra++;
        }
    }

    if (extra == 0) {
        return nullptr;
    }


    double missingValue = field.missingValue();
    size                = latitudes.size();

    for (size_t i = 0; i < field.dimensions(); i++) {
        MIRValuesVector newvalues(size, missingValue);
        const MIRValuesVector& values = field.direct(i);
        ASSERT(values.size() < size);

        for (size_t j = 0; j < values.size(); ++j) {
            newvalues[j] = values[j];
        }

        field.update(newvalues, i);
    }

    field.hasMissing(true);


    return new UnstructuredGrid(latitudes, longitudes);
}


static util::once_flag once;
static util::recursive_mutex* local_mutex               = nullptr;
static std::map<std::string, RepresentationFactory*>* m = nullptr;
static void init() {
    local_mutex = new util::recursive_mutex();
    m           = new std::map<std::string, RepresentationFactory*>();
}


RepresentationFactory::RepresentationFactory(const std::string& name) : name_(name) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("RepresentationFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


RepresentationFactory::~RepresentationFactory() {
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    m->erase(name_);
}


const Representation* RepresentationFactory::build(const param::MIRParametrisation& params) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    std::string name;
    if (!params.get("gridType", name)) {
        throw exception::SeriousBug("RepresentationFactory: cannot get 'gridType'");
    }

    Log::debug() << "RepresentationFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "RepresentationFactory: unknown '" << name << "', choices are: ");
        throw exception::SeriousBug("RepresentationFactory: unknown '" + name + "'");
    }

    return j->second->make(params);
}


void RepresentationFactory::list(std::ostream& out) {
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace mir::repres
