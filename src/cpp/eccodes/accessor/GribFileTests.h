#ifndef _GRIB_FILE_TESTS_H_20230818_
#define _GRIB_FILE_TESTS_H_20230818_

#include <filesystem>

namespace eccodes::accessor {

void runTests(std::filesystem::path gribFile, std::filesystem::path gribLayoutFile);

}

#endif // _GRIB_FILE_TESTS_H_20230818_
