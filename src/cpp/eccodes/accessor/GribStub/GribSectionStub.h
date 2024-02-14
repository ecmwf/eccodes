#pragma once

#include <string>
#include <memory>

namespace eccodes::accessor {

struct GribSection
{
    //grib_accessor* owner;
    //grib_handle* h;                 /** < Handles of all accessors and buffer  */
    //grib_accessor* aclength;        /** < block of the length of the block     */
    //grib_block_of_accessors* block; /** < block                                */
    //grib_action* branch;            /** < branch that created the block        */
    size_t length;
    size_t padding;
};

using GribSectionPtr = std::shared_ptr<GribSection>;

}
