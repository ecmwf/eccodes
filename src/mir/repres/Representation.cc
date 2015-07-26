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

#include "mir/repres/Representation.h"


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


Representation::Representation(): count_(0) {
}


Representation::~Representation() {
}

void Representation::attach() const {
    count_++;
}

void Representation::detach() const {
    if (--count_ == 0) {
        delete this;
    }
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
    eckit::StrStream os;
    os << "Representation::setComplexPacking() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::setSimplePacking(grib_info &) const {
    eckit::StrStream os;
    os << "Representation::setSimplePacking() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::setSecondOrderPacking(grib_info &) const {
    eckit::StrStream os;
    os << "Representation::setSecondOrderPacking() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::validate(const std::vector<double> &) const {
    eckit::StrStream os;
    os << "Representation::validate() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::fill(grib_info &) const {
    eckit::StrStream os;
    os << "Representation::fill() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::shape(size_t &ni, size_t &nj) const {
    eckit::StrStream os;
    os << "Representation::shape() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

const Representation *Representation::crop(const util::BoundingBox &bbox,
        const std::vector<double> &, std::vector<double> &) const {
    // FIXME: This is temporary, so that we can test existing mars/prodgen requeste
#if 0
    eckit::StrStream os;
    os << "Representation::crop() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
#else
    eckit::Log::info() << "Representation::crop() not implemented for " << *this << std::endl;
    return 0;
#endif
}


const Representation *Representation::truncate(size_t truncation,
        const std::vector<double> &, std::vector<double> &) const {
    eckit::StrStream os;
    os << "Representation::truncate() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}


atlas::Grid *Representation::atlasGrid() const {
    eckit::StrStream os;
    os << "Representation::atlasGrid() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

size_t Representation::truncation() const {
    eckit::StrStream os;
    os << "Representation::truncation() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

size_t Representation::frame(std::vector<double> &values, size_t size, double missingValue) const {
    eckit::StrStream os;
    os << "Representation::frame() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::reorder(long scanningMode, std::vector<double> &values) const {
    eckit::StrStream os;
    os << "Representation::reorder() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::cropToDomain(const param::MIRParametrisation &parametrisation, data::MIRField &) const {
    eckit::StrStream os;
    os << "Representation::cropToDomain() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::checkerboard(std::vector<double> &,bool hasMissing, double missingValue, bool normalize) const {
    eckit::StrStream os;
    os << "Representation::checkerboard() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::pattern(std::vector<double> &,bool hasMissing, double missingValue, bool normalize) const {
    eckit::StrStream os;
    os << "Representation::pattern() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}

void Representation::windDirections(const util::Rotation&, std::vector<double> &) const {
    eckit::StrStream os;
    os << "Representation::windDirections() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
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

    eckit::Log::info() << "Looking for RepresentationFactory [" << name << "]" << std::endl;

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

