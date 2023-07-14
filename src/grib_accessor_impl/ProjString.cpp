#include "ProjString.h"
#include "proj_string_helper.h"
#include "AccessorMaker.h"

//#include <iostream>

namespace eccodes {

ProjString::ProjString(grib_section* p, grib_action* creator, const long len, grib_arguments* arg) :
    Generic(p, creator, len, arg)
{
//    std::cout << "ProjString() addr=0x" << std::hex << this << '\n';
    grib_handle* h  = grib_handle_of_accessor(*this);

    this->grid_type = grib_arguments_get_name(h, arg, 0);
    this->endpoint  = grib_arguments_get_long(h, arg, 1);
    att.length       = 0;
    att.flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    att.flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

ProjString::~ProjString()
{
//    std::cout << "~ProjString() addr=0x" << std::hex << this << '\n';
}

int nativeType(ProjString& projString)
{
    return GRIB_TYPE_STRING;
}

#define ENDPOINT_SOURCE 0
#define ENDPOINT_TARGET 1

int unpackString(ProjString& projString, char* v, size_t* len)
{
    int err = 0, found = 0;
    size_t i           = 0;
    char grid_type[64] = {0,};
    grib_handle* h = grib_handle_of_accessor(projString);
    size_t size    = sizeof(grid_type) / sizeof(*grid_type);

    Assert(projString.endpoint == ENDPOINT_SOURCE || projString.endpoint == ENDPOINT_TARGET);

    err = grib_get_string(h, projString.grid_type, grid_type, &size);
    if (err) return err;

    proj_func func{};
    err = get_proj_func(grid_type, func);
    if(err == GRIB_SUCCESS)
    {
        if (projString.endpoint == ENDPOINT_SOURCE) {
            snprintf(v, 64, "EPSG:4326");
        }
        else {
            // Invoke the appropriate function to get the target proj string
            if ((err = func(h, v)) != GRIB_SUCCESS) return err;
        }
    }
    else if(err == GRIB_NOT_FOUND)
    {
        *len = 0;
        return GRIB_NOT_FOUND;
    }
    else return err;

    size = strlen(v);
    Assert(size > 0);
    *len = size + 1;
    return err;
}

static AccessorMaker projStringMaker("proj_string", AccessorType<ProjString>{});

}
