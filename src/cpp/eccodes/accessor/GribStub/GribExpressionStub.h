#pragma once

// C++ stub implementation of grib_expression
#include "GribCpp/GribStatus.h"
#include "AccessorDefs.h"
#include <memory>
#include <string>

namespace eccodes::accessor {

class GribExpression{};
using GribExpressionPtr = std::shared_ptr<GribExpression>;

class GribValues{};
using GribValuesPtr = std::shared_ptr<GribValues>;

GribStatus gribExpressionNativeType(GribExpressionPtr g);
GribStatus gribExpressionEvaluateLong(GribExpressionPtr g, long& result);
GribStatus gribExpressionEvaluateDouble(GribExpressionPtr g, double& result);
std::string gribExpressionEvaluateString(GribExpressionPtr g, std::string buf, int& err);
std::string gribExpressionGetName(GribExpressionPtr g);
void gribExpressionPrint(GribExpressionPtr g);
void gribExpressionFree(GribExpressionPtr g);
void gribExpressionAddDependency(GribExpressionPtr e, AccessorPtr observer);
GribStatus gribExpressionSetValue(GribExpressionPtr g, GribValuesPtr v);

}
