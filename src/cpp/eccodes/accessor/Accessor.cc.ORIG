/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "cpp/eccodes/accessor/Accessor.h"
#include <map>
#include <sstream>

namespace eccodes
{
namespace accessor
{


#if CODE_USING_ECKIT

int Accessor::pack_bytes(const unsigned char* val, size_t* len)
{
    std::ostringstream os;
    os << "Accessor::pack_bytes() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

int Accessor::pack_double(const double* v, size_t* len)
{
    std::ostringstream os;
    os << "Accessor::pack_double() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

int Accessor::pack_expression(grib_expression* e)
{
    std::ostringstream os;
    os << "Accessor::pack_expression() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

int Accessor::pack_long(const long* v, size_t* len)
{
    std::ostringstream os;
    os << "Accessor::pack_long() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

int Accessor::pack_string(const char* v, size_t* len)
{
    std::ostringstream os;
    os << "Accessor::pack_string() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}

int Accessor::pack_string_array(const char** v, size_t* len)
{
    std::ostringstream os;
    os << "Accessor::pack_string() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}



static util::once_flag once;
static util::recursive_mutex* local_mutex         = nullptr;
static std::map<std::string, AccessorFactory*>* m = nullptr;

static void init()
{
    local_mutex = new util::recursive_mutex();
    m           = new std::map<std::string, AccessorFactory*>();
}


AccessorFactory::AccessorFactory(const std::string& name) :
    name_(name)
{
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);
    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("AccessorFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}

AccessorFactory::~AccessorFactory()
{
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);
    m->erase(name_);
}


Accessor* AccessorFactory::build(std::string& name, long length, grib_arguments* args)
{
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);
    std::string name;
    if (!params.get("gridType", name)) {
        throw exception::SeriousBug("AccessorFactory: cannot get 'gridType'");
    }

    Log::debug() << "AccessorFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "AccessorFactory: unknown '" << name << "', choices are: ");
        throw exception::SeriousBug("AccessorFactory: unknown '" + name + "'");
    }

    return j->second->make(length, args);
}


void AccessorFactory::list(std::ostream& out)
{
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}
#endif


}  // namespace accessor
}  // namespace eccodes
