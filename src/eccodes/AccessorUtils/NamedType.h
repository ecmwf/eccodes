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

// https://www.fluentcpp.com/2016/12/08/strong-types-for-strong-interfaces/

#include <utility>

namespace eccodes
{

template <typename T, typename Parameter>
class NamedType
{
public:
    explicit NamedType(T const& value) : value_(value) {}
    explicit NamedType(T&& value) : value_(std::move(value)) {}
    T& get() { return value_; }
    T const& get() const {return value_; }

    bool hasValue() { return value_ != T{}; }

    friend bool operator<(const NamedType& l, const NamedType& r) { return l.value_ < r.value_; }

private:
    T value_;
};

} // namespace eccodes
