#pragma once

// Utilities provided to help get the C++ build working
//
// May be replaced with ECKit or other code at a later date

#include <exception>
#include <string>

//namespace eccodes::accessor {

using GribStatus = int;

class AccessorException : public std::exception{
std::string what_;
    GribStatus code_{};

public:
    AccessorException(int code) : code_{code} {}
    AccessorException(std::string const& what) : what_{what} {}

    const char* what() const noexcept override { return what_.c_str(); }
    GribStatus code() const { return code_; }

};

//}

