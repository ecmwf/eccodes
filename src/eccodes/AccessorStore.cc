/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "AccessorStore.h"

namespace eccodes {

void AccessorStore::add(const std::string& name, Accessor* accessor)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    store_[name] = accessor;
}

Accessor* AccessorStore::get(const std::string& name) const
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    auto it = store_.find(name);
    if (it != store_.end()) {
        return it->second;
    }
    return nullptr;
}

void AccessorStore::remove(const std::string& name)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    store_.erase(name);
}

void AccessorStore::clear()
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    store_.clear();
}

void AccessorStore::for_each(std::function<void(Accessor*)> fn) const
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    for (const auto& entry : store_) {
        fn(entry.second);
    }
}

} // namespace eccodes
