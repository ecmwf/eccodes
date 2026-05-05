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
class BuilderBase
{
    using Type = NamedType<std::string, struct TypeTag>;
    Type type_;
    using Ptr = T*;

public:
    BuilderBase(Type const&);
    virtual ~BuilderBase();
    virtual Ptr make() = 0;
};


template <class T, class ACCESSOR_DATA_TYPE>
class Builder : public BuilderBase<T>
{
    using Ptr = T*;
    Ptr make() override
    {
        return new ACCESSOR_DATA_TYPE();
    }

public:
    Builder() :
        BuilderBase<T>(ACCESSOR_DATA_TYPE::accessor_type_) {}
};


template <class T>
class Factory
{
public:
    static Factory& instance();
    using Ptr  = T*;
    using Type = NamedType<std::string, struct TypeTag>;

    ~Factory();

    void add(Type const& type, BuilderBase<T>* builder);
    void remove(Type const& type);
    bool has(Type const& type);
    void list(std::ostream&);

    Ptr build(Type const& type);


private:
    Factory() {}
    std::unordered_map<Type, BuilderBase<T>*> builders_;
    sync::Mutex mutex_;
};


template <class T>
Factory<T>::~Factory()
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    if (!builders_.empty()) {
        for (auto it = builders_.begin(); it != builders_.end();) {
            const grib_context* context = grib_context_get_default();
            grib_context_log(context, GRIB_LOG_DEBUG, "Factory::~Factory - Erasing %s", it->first.c_str());
            it = builders_.erase(it);
        }
    }
}

template <class T>
Factory<T>& Factory<T>::instance()
{
    static Factory theOne;
    return theOne;
}

template <class T>
void Factory<T>::add(Type const& type, BuilderBase<T>* builder)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
#if ECCODED_DEBUG
    if (has(type)) {
        throw std::runtime_error(std::string("Factory::add - duplicate entry: ") + type.c_str());
    }
#endif
    builders_[type] = builder;
}

template <class T>
void Factory<T>::remove(Type const& type)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    builders_.erase(type);
}

template <class T>
bool Factory<T>::has(Type const& type)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    return builders_.find(type) != builders_.end();
}

template <class T>
void Factory<T>::list(std::ostream& out)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);
    const grib_context* context = grib_context_get_default();
    for (auto const& entry : builders_) {
        grib_context_log(context, GRIB_LOG_DEBUG, "%s, ", entry.first.c_str());
    }
}

template <class T>
typename Factory<T>::Ptr Factory<T>::build(Type const& type)
{
    sync::LockGuard<sync::Mutex> guard(mutex_);

    if (auto builder_ = builders_.find(type); builder_ == builders_.end()) {
        const grib_context* context = grib_context_get_default();
        grib_context_log(context, GRIB_LOG_ERROR, "No Builder called %s", type.c_str());
        grib_context_log(context, GRIB_LOG_ERROR, "Builders are:");
        for (auto const& entry : builders_) {
            grib_context_log(context, GRIB_LOG_ERROR, "No Builder called %s", entry.first.c_str());
        }
        throw std::runtime_error(std::string("No Builder called ") + type.c_str());
    }
    else {
        auto builder =  builder_->second->make();
        // eccodes::AccessorStore::instance().add(builder);
        return builder;
    }
}

template <class T>
BuilderBase<T>::BuilderBase(Type const& type) :
    type_(type)
{
    Factory<T>::instance().add(type_, this);
}

template <class T>
BuilderBase<T>::~BuilderBase()
{
    Factory<T>::instance().remove(type_);
}

} // namespace eccodes
