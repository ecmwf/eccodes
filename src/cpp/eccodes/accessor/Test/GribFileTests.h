#pragma once

#include <filesystem>

namespace eccodes::accessor {

void runTests(std::filesystem::path gribFile, std::filesystem::path gribLayoutFile);

}

