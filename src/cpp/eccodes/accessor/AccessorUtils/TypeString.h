#pragma once

// Utilities provided to help get the C++ build working
//
// May be replaced with ECKit or other code at a later date

#include <string>

namespace eccodes::accessor {

template<typename T> std::string typeString() { return "unknown"; }
template<> std::string typeString<long>() { return "long"; }
template<> std::string typeString<double>() { return "double"; }
template<> std::string typeString<float>() { return "float"; }
template<> std::string typeString<char>() { return "char"; }


}

