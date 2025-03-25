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

// Utilities provided to help get the C++ build working
// May be replaced with EcKit or other code at a later date

#include <exception>
#include <string>

namespace eccodes
{

using GribStatus = int;

class Exception : public std::exception{
public:
    Exception(int code) : code_{code} {}
    Exception(std::string const& what) : what_{what} {}

    const char* what() const noexcept override { return what_.c_str(); }
    GribStatus code() const { return code_; }
private:
    std::string what_;
    GribStatus code_{};
};

} // namespace eccodes
