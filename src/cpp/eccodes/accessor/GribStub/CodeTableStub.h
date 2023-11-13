#pragma once

#include <string>
#include <array>
#include <vector>
#include <memory>

namespace eccodes::accessor {

#define MAX_SMART_TABLE_COLUMNS 20

struct CodeTableEntry
{
    std::string abbreviation;
    std::string title;
    std::string units;
};

struct GribCodetable
{
    std::array<std::string,2> filename;
    std::array<std::string,2> recomposed_name;
    std::shared_ptr<GribCodetable> next;
    size_t size;
    std::array<CodeTableEntry,2> entries;
};

using GribCodeTablePtr = std::shared_ptr<GribCodetable>;

struct GribSmartTableEntry
{
    std::string abbreviation;
    std::array<std::string, MAX_SMART_TABLE_COLUMNS> column;
};

struct GribSmartTable
{
    std::array<std::string,3> filename;
    std::array<std::string,3> recomposed_name;
    std::shared_ptr<GribSmartTable> next;
    size_t numberOfEntries;
    std::shared_ptr<GribSmartTableEntry> entries;    // Would be better as a vector
};

}
