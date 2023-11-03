#pragma once

// Utilities provided to help get the C++ build working
//
// May be replaced with ECKit or other code at a later date

#include "GribCpp/GribStatus.h"
#include <exception>
#include <string>

namespace eccodes::accessor {

class AccessorException : public std::exception{
std::string what_;
GribStatus code_{};

public:
    AccessorException(GribStatus code) : code_{code} {}
    AccessorException(std::string const& what) : what_{what} {}

const char* what() const noexcept override { return what_.c_str(); }
GribStatus code() const { return code_; }

};

}

