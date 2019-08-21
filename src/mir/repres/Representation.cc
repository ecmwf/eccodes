/*
 * (C) Copyright 1996- ECMWF.
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


#include "mir/repres/Representation.h"

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"

#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/namedgrids/NamedGrid.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/other/UnstructuredGrid.h"
#include "mir/util/Domain.h"


namespace mir {
namespace repres {


Representation::Representation() {
}


Representation::~Representation() = default;

RepresentationHandle::RepresentationHandle(const Representation *representation):
    representation_(representation) {
    if (representation_) {
        representation_->attach();
    }
}


RepresentationHandle::~RepresentationHandle() {
    if (representation_) {
        representation_->detach();
    }
}


void Representation::setComplexPacking(grib_info&) const {
    std::ostringstream os;
    os << "Representation::setComplexPacking() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::setSimplePacking(grib_info&) const {
    std::ostringstream os;
    os << "Representation::setSimplePacking() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::setGivenPacking(grib_info&) const {
    std::ostringstream os;
    os << "Representation::setGivenPacking() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::estimate(api::MIREstimation&) const {
    std::ostringstream os;
    os << "Representation::estimate() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Representation::isGlobal() const {
    bool global = isPeriodicWestEast() &&
                  includesNorthPole() &&
                  includesSouthPole();

    ASSERT(global == domain().isGlobal());
    return global;
}


bool Representation::isPeriodicWestEast() const {
    std::ostringstream os;
    os << "Representation::isPeriodicWestEast() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Representation::includesNorthPole() const {
    std::ostringstream os;
    os << "Representation::includesNorthPole() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Representation::includesSouthPole() const {
    std::ostringstream os;
    os << "Representation::includesSouthPole() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::validate(const MIRValuesVector&) const {
    std::ostringstream os;
    os << "Representation::validate() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::fill(grib_info&) const {
    std::ostringstream os;
    os << "Representation::fill(grib_info&) not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::fill(api::MIRJob&) const {
    std::ostringstream os;
    os << "Representation::fill(api::MIRJob&) not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::fill(util::MeshGeneratorParameters&) const {
    std::ostringstream os;
    os << "Representation::fill(util::MeshGeneratorParameters&) not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


const Representation *Representation::croppedRepresentation(const util::BoundingBox&) const {
    std::ostringstream os;
    os << "Representation::croppedRepresentation() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


const Representation *Representation::truncate(size_t, const MIRValuesVector&, MIRValuesVector&) const {
    std::ostringstream os;
    os << "Representation::truncate() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
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


void Representation::makeName(std::ostream&) const {
    std::ostringstream os;
    os << "Representation::makeName(std::ostream& out) not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Representation::sameAs(const Representation&) const {
    std::ostringstream os;
    os << "Representation::sameAs() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

atlas::Grid Representation::atlasGrid() const {
    std::ostringstream os;
    os << "Representation::atlasGrid() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


util::Domain Representation::domain() const {
    std::ostringstream os;
    os << "Representation::domain() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


const util::BoundingBox& Representation::boundingBox() const {
    std::ostringstream os;
    os << "Representation::boundingBox() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


util::BoundingBox Representation::extendBoundingBox(const util::BoundingBox&) const {
    std::ostringstream os;
    os << "Representation::extendBoundingBox() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Representation::extendBoundingBoxOnIntersect() const {
    std::ostringstream os;
    os << "Representation::extendBoundingBoxOnIntersect() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


size_t Representation::truncation() const {
    std::ostringstream os;
    os << "Representation::truncation() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


size_t Representation::numberOfPoints() const {
    std::ostringstream os;
    os << "Representation::numberOfPoints() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Representation::getLongestElementDiagonal(double&) const {
    std::ostringstream os;
    os << "Representation::getLongestElementDiagonal() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::comparison(std::string&) const {
    // do nothing
}


size_t Representation::frame(MIRValuesVector&, size_t, double) const {
    std::ostringstream os;
    os << "Representation::frame() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

std::string Representation::factory() const {
    std::ostringstream os;
    os << "Representation::factory() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Representation::reorder(long, MIRValuesVector&) const {
    std::ostringstream os;
    os << "Representation::reorder() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


Iterator *Representation::iterator() const {
    std::ostringstream os;
    os << "Representation::iterator() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


const Representation* Representation::globalise(data::MIRField& field) const {
    const util::Domain dom = domain();

    if (dom.isGlobal()) {
        return 0;
    }

    // TODO: cache me

    RepresentationHandle octahedral(namedgrids::NamedGrid::lookup("O320").representation());
    size_t size = octahedral->numberOfPoints() + numberOfPoints();

    std::vector<double> latitudes;  latitudes.resize(size);
    std::vector<double> longitudes; longitudes.resize(size);

    std::unique_ptr<repres::Iterator> it(octahedral->iterator());
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
        return 0;
    }


    double missingValue = field.missingValue();
    size = latitudes.size();

    for (size_t i = 0; i < field.dimensions(); i++ ) {
        MIRValuesVector newvalues(size, missingValue);
        const MIRValuesVector& values = field.direct(i);
        ASSERT(values.size() < size);

        for (size_t j = 0 ; j < values.size(); ++j) {
            newvalues[j] = values[j];
        }

        field.update(newvalues, i);
    }

    field.hasMissing(true);


    return new other::UnstructuredGrid(latitudes, longitudes);
}


//=========================================================================


namespace {
static pthread_once_t once = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex = 0;
static std::map< std::string, RepresentationFactory* >* m = 0;
static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map< std::string, RepresentationFactory* >();
}
}  // (anonymous namespace)


RepresentationFactory::RepresentationFactory(const std::string& name):
    name_(name) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(name) != m->end()) {
        throw eckit::SeriousBug("RepresentationFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


RepresentationFactory::~RepresentationFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    m->erase(name_);
}


const Representation* RepresentationFactory::build(const param::MIRParametrisation& params) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    std::string name;
    if (!params.get("gridType", name)) {
        throw eckit::SeriousBug("RepresentationFactory: cannot get 'gridType'");
    }

    eckit::Log::debug<LibMir>() << "RepresentationFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(eckit::Log::error() << "RepresentationFactory: unknown '" << name << "', choices are: ");
        throw eckit::SeriousBug("RepresentationFactory: unknown '" + name + "'");
    }

    return (*j).second->make(params);
}


void RepresentationFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace repres
}  // namespace mir

