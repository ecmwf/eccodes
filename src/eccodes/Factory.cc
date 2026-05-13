/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Factory.h"
#include <cstdio>
#include <cstdlib>

namespace eccodes {

Factory& Factory::instance() {
    static Factory theOne;
    return theOne;
}

void Factory::add(const char* name, AccessorCreator creator) {
    std::size_t h = hash(name);
    store_[h] = Entry{creator, name};
}

Accessor* Factory::build(const char* name) const {
    std::size_t h = hash(name);
    auto it = store_.find(h);
    if (it != store_.end()) {
        return it->second.creator();
    }
    fprintf(stderr, "FATAL: Factory::build - unknown accessor type '%s'\n", name);
    std::abort();
    return nullptr;
}

bool Factory::has(const char* name) const {
    std::size_t h = hash(name);
    return store_.find(h) != store_.end();
}

}  // namespace eccodes
