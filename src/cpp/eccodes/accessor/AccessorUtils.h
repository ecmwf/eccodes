#pragma once

// Utilities provided to help get the C++ build working
//
// May be replaced with ECKit or other code at a later date

#include <exception>
#include <fstream>

namespace eccodes::accessor {

class AccessorException : public std::exception{
std::string what_;
int code_{};

public:
    AccessorException(int code) : code_{code} {}
    AccessorException(std::string const& what) : what_{what} {}

const char* what() const noexcept override { return what_.c_str(); }
int code() const { return code_; }

};

std::ofstream& debugLog();
std::ofstream& errorLog();

}

