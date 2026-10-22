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

#include "AccessorUtils/NamedType.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace eccodes
{

// Logs the unknown type together with the most similar known types, then throws
[[noreturn]] void throwNoCreator(const char* type, const std::vector<const char*>& knownTypes);

// Creates objects of the classes derived from T, by the name of their type.
//
// To use it for a new T, define the constructor and register every class in it:
//
//     template <> CodesFactory<T>::CodesFactory() { add<A>(); add<B>(); }
//
// and declare this specialisation wherever the factory is used.
// The classes are registered explicitly, in one place, because classes which register
// themselves from their own source files get lost when the library is static: the linker
// drops the object files that nothing refers to.
template <class T>
class CodesFactory
{
public:
    // The name of a type. It refers to a string literal, so it is a compile-time constant
    using Type = NamedType<const char*, struct TypeTag>;

    // The factory with all classes registered
    static const CodesFactory& instance()
    {
        static const CodesFactory theOne;
        return theOne;
    }

    // Throws if no class is registered under this type
    T* build(const Type& type) const
    {
        if (auto it = creators_.find(type); it != creators_.end()) {
            return it->second();
        }
        std::vector<const char*> knownTypes;
        for (const auto& entry : creators_) {
            knownTypes.push_back(entry.first.c_str());
        }
        throwNoCreator(type.c_str(), knownTypes);
    }

private:
    CodesFactory();

    // Registers the class C under the type C::accessor_type_
    template <class C>
    void add()
    {
        creators_[C::accessor_type_] = []() -> T* { return new C(); };
    }

    std::unordered_map<Type, T* (*)()> creators_;
};

}  // namespace eccodes
