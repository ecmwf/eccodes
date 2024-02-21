#pragma once

#include "AccessorDefs.h"

#include <string>
#include <vector>

namespace eccodes::accessor {

/* types of BUFR descriptors used in bufr_descriptor->type*/

enum class BufrDescriptorType{
    UNKNOWN,
    STRING,
    DOUBLE,
    LONG,
    TABLE,
    FLAG,
    REPLICATION,
    OPERATOR,
    SEQUENCE,

    // Used for conversion routines - keep last!
    ENUMSIZE
};

constexpr int toInt(BufrDescriptorType type) {
    return static_cast<int>(type);
}

//std::string toString(GribType type);
//GribType toGribType(std::string_view type);

struct BufrDescriptor
{
    //grib_context* context;
    long code;
    int F;
    int X;
    int Y;
    int type;
    /*char* name;   Not needed: All usage commented out. See ECC-489 */
    std::string shortName;
    std::string units;
    long scale;
    double factor;
    long reference;
    long width;
    int nokey; /* set if descriptor does not have an associated key */
    AccessorPtr a; //grib_accessor* a;
};

struct BufrDescriptorsArray
{
    std::vector<std::vector<BufrDescriptor>> v;
    size_t size;
    size_t n;
    size_t incsize;
    size_t number_of_pop_front;
    //grib_context* context;
};

struct bufr_descriptors_map_list
{
    std::vector<BufrDescriptor> unexpanded;
    std::vector<BufrDescriptor> expanded;
    bufr_descriptors_map_list* next;
};

/* BUFR: Operator 203YYY: Linked list storing Table B changed reference values */
struct BufrTablebOverride
{
    BufrTablebOverride* next;
    int code;
    long newRefVal;
};


}
