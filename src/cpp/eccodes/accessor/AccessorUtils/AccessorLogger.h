#pragma once

// Logging utilities provided to help get the C++ build working
//
// May be replaced with ECKit or other code at a later date

#include <fstream>

namespace eccodes::accessor {

std::ofstream& debugLog();
std::ofstream& errorLog();

}

