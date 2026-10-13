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

#include <cstddef>
#include <functional>
#include <string>
#include <utility>

namespace eccodes
{

constexpr std::size_t fnv1a_hash(const char* str)
{
    std::size_t hash = 14695981039346656037ULL;
    while (*str) {
        hash ^= static_cast<std::size_t>(*str++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

namespace detail
{
constexpr const char* c_str(const char* value) { return value; }
inline const char* c_str(const std::string& value) { return value.c_str(); }
} // namespace detail

// With T = const char* the type is a compile-time constant (constexpr). It then does not
// own the string: the string must outlive it, as a string literal does.
template <typename T, typename Parameter>
class NamedType
{
public:
    explicit constexpr NamedType(T const& value) : value_(value), uid_(fnv1a_hash(detail::c_str(value_))) {}
    explicit constexpr NamedType(T&& value) : value_(std::move(value)), uid_(fnv1a_hash(detail::c_str(value_))) {}

    T& get() { return value_; }
    constexpr T const& get() const { return value_; }
    constexpr const char* c_str() const { return detail::c_str(value_); }
    constexpr std::size_t uid() const { return uid_; }

    friend constexpr bool operator==(const NamedType& l, const NamedType& r) { return l.uid_ == r.uid_; }
    friend constexpr bool operator!=(const NamedType& l, const NamedType& r) { return l.uid_ != r.uid_; }
    friend constexpr bool operator<(const NamedType& l, const NamedType& r) { return l.uid_ < r.uid_; }

    friend constexpr bool operator==(const NamedType& l, const char* r) { return l.uid_ == fnv1a_hash(r); }
    friend constexpr bool operator==(const char* l, const NamedType& r) { return fnv1a_hash(l) == r.uid_; }
    friend constexpr bool operator!=(const NamedType& l, const char* r) { return l.uid_ != fnv1a_hash(r); }
    friend constexpr bool operator!=(const char* l, const NamedType& r) { return fnv1a_hash(l) != r.uid_; }

private:
    T value_;
    std::size_t uid_;
};

} // namespace eccodes

namespace std
{

template <typename T, typename Parameter>
struct hash<eccodes::NamedType<T, Parameter>> {
    std::size_t operator()(const eccodes::NamedType<T, Parameter>& nt) const {
        return nt.uid();
    }
};

} // namespace std
