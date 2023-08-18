#ifndef _GRIB_FILE_H_20230817_
#define _GRIB_FILE_H_20230817_

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

#endif // _GRIB_FILE_H_20230817_
