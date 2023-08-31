#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace eccodes::accessor {

using GribBuffer = std::vector<char>;

std::ostream& operator<<(std::ostream& os, GribBuffer const& buffer);

void createGribFile(std::filesystem::path gribFile);
GribBuffer parseGribFile(std::filesystem::path gribFile);

}

