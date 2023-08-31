#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace eccodes::accessor {

struct LayoutEntry{
    std::string name_;
    std::string type_;
    int byteCount_{};
};

using LayoutEntries = std::vector<LayoutEntry>;

std::ostream& operator<<(std::ostream& os, LayoutEntries entries);

LayoutEntries parse_grib_layout(std::filesystem::path layoutFile);

}

