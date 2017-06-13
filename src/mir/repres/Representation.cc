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


#include "mir/repres/Representation.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "atlas/grid.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/MIRGrid.h"

#include "mir/namedgrids/NamedGrid.h"
#include "mir/repres/other/UnstructuredGrid.h"
#include "mir/repres/Iterator.h"
#include "mir/data/MIRField.h"

namespace mir {
namespace repres {
namespace {


static eckit::Mutex *local_mutex = 0;
static std::map<std::string, RepresentationFactory *> *m = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map<std::string, RepresentationFactory *>();
}


}  // (anonymous namespace)


Representation::Representation() {
}


Representation::~Representation() {
}

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


void Representation::setComplexPacking(grib_info &) const {
    std::ostringstream os;
    os << "Representation::setComplexPacking() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::setSimplePacking(grib_info &) const {
    std::ostringstream os;
    os << "Representation::setSimplePacking() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::setGivenPacking(grib_info &) const {
    std::ostringstream os;
    os << "Representation::setGivenPacking() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool Representation::isGlobal() const {
    return isPeriodicWestEast() &&
            includesNorthPole() &&
            includesSouthPole();
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


void Representation::validate(const std::vector<double> &) const {
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


void Representation::shape(size_t &ni, size_t &nj) const {
    std::ostringstream os;
    os << "Representation::shape() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


const Representation *Representation::cropped(const util::BoundingBox &bbox) const {
    std::ostringstream os;
    os << "Representation::cropped() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


const Representation *Representation::truncate(size_t truncation,
        const std::vector<double> &, std::vector<double> &) const {
    std::ostringstream os;
    os << "Representation::truncate() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

const std::string& Representation::uniqueName() const {
    if(uniqueName_.empty()) {
        std::ostringstream os;
        makeName(os);
        uniqueName_ = os.str();
    }
    return uniqueName_;
}

void Representation::makeName(std::ostream& out) const {
    std::ostringstream os;
    os << "Representation::makeName(std::ostream& out) not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

bool Representation::sameAs(const Representation& other) const {
    std::ostringstream os;
    os << "Representation::sameAs() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

util::MIRGrid Representation::grid() const {
    return util::MIRGrid(atlasGrid(), domain());
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

size_t Representation::truncation() const {
    std::ostringstream os;
    os << "Representation::truncation() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


size_t Representation::pentagonalResolutionTs() const {
    std::ostringstream os;
    os << "Representation::pentagonalResolutionTs() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

size_t Representation::numberOfPoints() const {
    std::ostringstream os;
    os << "Representation::numberOfPoints() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Representation::comparison(std::string&) const {
    // do nothing
}


size_t Representation::frame(std::vector<double> &values, size_t size, double missingValue) const {
    std::ostringstream os;
    os << "Representation::frame() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


const Representation* Representation::subset(data::MIRField& field,
        const util::Increments& increments) const {
    std::ostringstream os;
    os << "Representation::subset() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Representation::reorder(long scanningMode, std::vector<double> &values) const {
    std::ostringstream os;
    os << "Representation::reorder() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Representation::cropToDomain(const param::MIRParametrisation &parametrisation, context::Context & ctx) const {
    std::ostringstream os;
    os << "Representation::cropToDomain() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


Iterator *Representation::unrotatedIterator() const {
    std::ostringstream os;
    os << "Representation::unrotatedIterator() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


Iterator *Representation::rotatedIterator() const {
    std::ostringstream os;
    os << "Representation::rotatedIterator() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


//=========================================================================

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

    eckit::ScopedPtr<repres::Iterator> iter(octahedral->unrotatedIterator());

    repres::Iterator::value_type lat;
    repres::Iterator::value_type lon;


    iter.reset(unrotatedIterator());
    while (iter->next(lat, lon)) {
        latitudes.push_back(lat);
        longitudes.push_back(lon);
    }

    size_t extra = 0;
    while (iter->next(lat, lon)) {
        if (!dom.contains(lat, lon)) {
            latitudes.push_back(lat);
            longitudes.push_back(lon);
            extra++;
        }
    }

    if (extra == 0) {
        return 0;
    }


    double missingValue = field.missingValue();
    size = latitudes.size();

    for (size_t i = 0; i < field.dimensions(); i++ ) {
        std::vector<double> newvalues(size, missingValue);
        const std::vector<double> &values = field.direct(i);
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


RepresentationFactory::RepresentationFactory(const std::string &name):
    name_(name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


RepresentationFactory::~RepresentationFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}


const Representation *RepresentationFactory::build(const param::MIRParametrisation &params) {

    pthread_once(&once, init);


    std::string name;

    if (!params.get("gridType", name)) {
        throw eckit::SeriousBug("RepresentationFactory cannot get gridType");
    }

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    std::map<std::string, RepresentationFactory *>::const_iterator j = m->find(name);

    // eckit::Log::debug<LibMir>() << "Looking for RepresentationFactory [" << name << "]" << std::endl;

    if (j == m->end()) {
        eckit::Log::error() << "No RepresentationFactory for [" << name << "]" << std::endl;
        eckit::Log::error() << "RepresentationFactories are:" << std::endl;
        for (j = m->begin() ; j != m->end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No RepresentationFactory called ") + name);
    }

    return (*j).second->make(params);
}


}  // namespace repres
}  // namespace mir

