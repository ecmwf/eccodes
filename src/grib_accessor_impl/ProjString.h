#ifndef _PROJ_STRING_H_20230705_
#define _PROJ_STRING_H_20230705_

#include "Generic.h"

namespace eccodes {

struct ProjString : public Generic
{
    const char* grid_type{};
    int endpoint{};

    ProjString(grib_section* p, grib_action* creator, const long len, grib_arguments* arg);
    virtual ~ProjString();
};

int nativeType(ProjString& projString);
int unpackString(ProjString& projString, char* v, size_t* len);

}
#endif // _PROJ_STRING_H_20230705_
