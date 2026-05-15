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

#include "grib_api_internal.h"
#include "AccessorUtils/NamedType.h"
#include "sync/Mutex.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace eccodes {

template <class T>
class CodesFactory
{
public:
    static CodesFactory& instance();
    using Ptr     = T*;
    using Type    = NamedType<std::string, struct TypeTag>;
    using Creator = Ptr(*)();

    void add(Type const& type, Creator creator);
    void remove(Type const& type);
    Ptr build(Type const& type);

private:
    CodesFactory() {}
    std::unordered_map<Type, Creator> creators_;
    sync::Mutex mutex_;
};


template <class T>
CodesFactory<T>& CodesFactory<T>::instance()
{
    static CodesFactory theOne;
    return theOne;
}

template <class T>
void CodesFactory<T>::add(Type const& type, Creator creator)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    creators_[type] = creator;
}

template <class T>
void CodesFactory<T>::remove(Type const& type)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    creators_.erase(type);
}

template <class T>
typename CodesFactory<T>::Ptr CodesFactory<T>::build(Type const& type)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);

    if (auto it = creators_.find(type); it == creators_.end()) {
        const grib_context* context = grib_context_get_default();
        grib_context_log(context, GRIB_LOG_ERROR, "No creator for type %s", type.c_str());

        // Find the 5 most similar registered types using Levenshtein distance
        constexpr size_t maxSuggestions = 5;
        std::vector<std::pair<size_t, const char*>> candidates;
        candidates.reserve(creators_.size());
        for (auto const& entry : creators_) {
            size_t dist = levenshteinDistance(type.c_str(), entry.first.c_str());
            candidates.push_back({dist, entry.first.c_str()});
        }
        std::partial_sort(candidates.begin(),
                          candidates.begin() + std::min(maxSuggestions, candidates.size()),
                          candidates.end(),
                          [](auto const& a, auto const& b) { return a.first < b.first; });
        size_t n = std::min(maxSuggestions, candidates.size());
        if (n > 0) {
            grib_context_log(context, GRIB_LOG_ERROR, "Did you mean:");
            for (size_t i = 0; i < n; ++i) {
                grib_context_log(context, GRIB_LOG_ERROR, "  %s", candidates[i].second);
            }
        }
        throw std::runtime_error(std::string("No creator for type ") + type.c_str());
    }
    else {
        return it->second();
    }
}


template <class T, class ConcreteType>
class Registrar
{
    using Type = typename CodesFactory<T>::Type;
    Type type_;

    static T* create() { return new ConcreteType(); }

public:
    Registrar() : type_(ConcreteType::accessor_type_)
    {
        CodesFactory<T>::instance().add(type_, &create);
    }
    ~Registrar()
    {
        CodesFactory<T>::instance().remove(type_);
    }
};

// Backward-compatible alias
template <class T, class ConcreteType>
using Builder = Registrar<T, ConcreteType>;

} // namespace eccodes
