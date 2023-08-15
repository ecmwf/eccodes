#include <iostream>
#include <vector>
#include "AccessorFactory.h"
#include "Accessor.h"
#include "grib_api_internal.h"

int main()
{
    std::cout << "Creating Uint8 Accessor...";

    grib_section gs;
    grib_action creator{.name="Test888", .op="uint8"};
    long len{1};
    grib_arguments arg;

    auto uint8Accessor = eccodes::accessor::makeAccessor(&gs, &creator, len, &arg);

    std::vector<long> data;

    std::cout << (uint8Accessor ? "SUCCESS!\n" : "FAILED!\n");
    uint8Accessor->pack(data);
    uint8Accessor->unpack(data);

    std::cout << '\n';

}