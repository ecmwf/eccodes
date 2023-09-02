#include "ProjStringData.h"
#include "AccessorFactory.h"
#include "AccessorStore.h"
#include "Accessor.h"
#include "AccessorUtils/AccessorProxy.h"
#include "grib_api_internal.h"

namespace eccodes::accessor {

ProjStringData::ProjStringData(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // For now we just "know" which argument to extract
    int argIndex{0};
    gridType_ = AccessorName(std::get<std::string>(initData[argIndex++].second));
    endpoint_ = std::get<long>(initData[argIndex++].second);

    // These are private base members - consider a redesign...
    // length_   = 0;
    // a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    // a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

GribType ProjStringData::nativeType() const
{
    return GribType::STRING;
}

constexpr static int endpointSource{0};
constexpr static int endpointTarget{1};
GribStatus ProjStringData::unpack(std::vector<char> &values) const
{
    return GribStatus::NOT_IMPLEMENTED;

#if 0

//static int unpack_string(grib_accessor* a, char* v, size_t* len)

    int err = 0, found = 0;
    size_t i           = 0;
    char grid_type[64] = {0,};
//    grib_handle* h = grib_handle_of_accessor(a);
    size_t size    = sizeof(grid_type) / sizeof(*grid_type);

    Assert(endpoint_ == endpointSource || endpoint_ == endpointTarget);

    auto gridTypeAccessor = AccessorStore::instance().getAccessor(gridType_);
    auto gridTypeString = gridTypeAccessor->unpack<char>();

    for (i = 0; !found && i < NUMBER(proj_mappings); ++i) {
        proj_mapping pm = proj_mappings[i];
        if (strcmp(grid_type, pm.gridType) == 0) {
            found = 1;
            if (self->endpoint == endpointSource) {
                snprintf(v, 64, "EPSG:4326");
            }
            else {
                // Invoke the appropriate function to get the target proj string
                if ((err = pm.func(h, v)) != GRIB_SUCCESS) return err;
            }
        }
    }
    if (!found) {
        *len = 0;
        return GRIB_NOT_FOUND;
    }

    size = strlen(v);
    Assert(size > 0);
    *len = size + 1;
    return err;

#endif
}

namespace {
AccessorBuilder<ProjStringData> ProjStringDataBuilder(AccessorType("proj_string"));
}

}

