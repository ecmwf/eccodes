/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "sync/Mutex.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace eccodes {

class Accessor;

class AccessorStore {
public:
    void add(const std::string& name, Accessor* accessor);
    Accessor* get(const std::string& name) const;
    void remove(const std::string& name);
    void clear();
    void for_each(std::function<void(Accessor*)> fn) const;

private:
    std::unordered_map<std::string, Accessor*> store_;
    mutable sync::Mutex mutex_;
};

} // namespace eccodes
