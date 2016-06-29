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


#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/Once.h"

#include "mir/param/MIRParametrisation.h"
#include "atlas/grid/Domain.h"

#include "mir/repres/Representation.h"
#include "mir/log/MIR.h"


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

void Representation::setSecondOrderPacking(grib_info &) const {
    std::ostringstream os;
    os << "Representation::setSecondOrderPacking() not implemented for " << *this;
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


atlas::grid::Grid *Representation::atlasGrid() const {
    std::ostringstream os;
    os << "Representation::atlasGrid() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

atlas::grid::Domain Representation::atlasDomain() const {
    std::ostringstream os;
    os << "Representation::atlasDomain() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

size_t Representation::truncation() const {
    std::ostringstream os;
    os << "Representation::truncation() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

size_t Representation::frame(std::vector<double> &values, size_t size, double missingValue) const {
    std::ostringstream os;
    os << "Representation::frame() not implemented for " << *this;
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

    eckit::Log::trace<MIR>() << "Looking for RepresentationFactory [" << name << "]" << std::endl;

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

