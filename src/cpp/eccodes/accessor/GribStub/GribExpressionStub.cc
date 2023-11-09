#include "GribExpressionStub.h"
#include "grib_api_internal.h"

namespace eccodes::accessor {

GribStatus gribExpressionNativeType(GribExpressionPtr g)
{
    Assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribExpressionEvaluateLong(GribExpressionPtr g, long& result)
{
    Assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribExpressionEvaluateDouble(GribExpressionPtr g, double& result)
{
    Assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

std::string gribExpressionEvaluateString(GribExpressionPtr g, std::string buf, int& err)
{
    Assert(false);
    return {};
}

std::string gribExpressionGetName(GribExpressionPtr g)
{
    Assert(false);
    return {};
}

void gribExpressionPrint(GribExpressionPtr g)
{
    Assert(false);
}

void gribExpressionFree(GribExpressionPtr g)
{
    Assert(false);
}

void gribExpressionAddDependency(GribExpressionPtr e, AccessorPtr observer)
{
    Assert(false);
}

GribStatus gribExpressionSetValue(GribExpressionPtr g, GribValuesPtr v)
{
    Assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}


}
