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
#include <stdexcept>
#include <unordered_map>

namespace eccodes {

template <class T>
class Factory
{
public:
    static Factory& instance();
    using Ptr     = T*;
    using Type    = NamedType<std::string, struct TypeTag>;
    using Creator = Ptr(*)();

    void add(Type const& type, Creator creator);
    void remove(Type const& type);
    Ptr build(Type const& type);

private:
    Factory() {}
    std::unordered_map<Type, Creator> creators_;
    sync::Mutex mutex_;
};


template <class T>
Factory<T>& Factory<T>::instance()
{
    static Factory theOne;
    return theOne;
}

template <class T>
void Factory<T>::add(Type const& type, Creator creator)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    creators_[type] = creator;
}

template <class T>
void Factory<T>::remove(Type const& type)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    creators_.erase(type);
}

template <class T>
typename Factory<T>::Ptr Factory<T>::build(Type const& type)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);

    if (auto it = creators_.find(type); it == creators_.end()) {
        const grib_context* context = grib_context_get_default();
        grib_context_log(context, GRIB_LOG_ERROR, "No creator for type %s", type.c_str());
        grib_context_log(context, GRIB_LOG_ERROR, "Registered types:");
        for (auto const& entry : creators_) {
            grib_context_log(context, GRIB_LOG_ERROR, "  %s", entry.first.c_str());
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
    using Type = typename Factory<T>::Type;
    Type type_;

    static T* create() { return new ConcreteType(); }

public:
    Registrar() : type_(ConcreteType::accessor_type_)
    {
        Factory<T>::instance().add(type_, &create);
    }
    ~Registrar()
    {
        Factory<T>::instance().remove(type_);
    }
};

// Backward-compatible alias
template <class T, class ConcreteType>
using Builder = Registrar<T, ConcreteType>;

} // namespace eccodes
