#pragma once

// C++ implementation of the functions in grib_accessor.cc that are not called from within the Accessor C++ classes

#include "AccessorDefs.h"
#include "AccessorData/AccessorInitData.h"
#include "GribExpressionStub.h"
#include "GribCpp/GribStatus.h"

namespace eccodes::accessor {

GribStatus gribInlineStrcmp(const std::string a, const std::string b);
GribStatus gribPackMissing(AccessorPtr ptr);
GribStatus gribPackZero(AccessorPtr ptr);
GribStatus gribIsMissingInternal(AccessorPtr ptr);
GribStatus gribPackDouble(AccessorPtr ptr, const std::vector<double>& values);
GribStatus gribPackDouble(AccessorPtr ptr, const double value);
GribStatus gribPackFloat(AccessorPtr ptr, const std::vector<float>& values);
GribStatus gribPackFloat(AccessorPtr ptr, const float value);
GribStatus gribPackExpression(AccessorPtr ptr, GribExpressionPtr e);
GribStatus gribPackString(AccessorPtr ptr, const std::string value);
GribStatus gribPackStringArray(AccessorPtr ptr, const std::vector<std::string>& values);
GribStatus gribPackLong(AccessorPtr ptr, const std::vector<long>& values);
GribStatus gribPackLong(AccessorPtr ptr, const long value);
GribStatus gribPackBytes(AccessorPtr ptr, const std::vector<unsigned char>& values);

GribStatus gribUnpackBytes(AccessorPtr ptr, std::vector<unsigned char>& values);
GribStatus gribUnpackDoubleSubarray(AccessorPtr ptr, std::vector<double>& v, size_t start);
GribStatus gribUnpackDouble(AccessorPtr ptr, std::vector<double>& values);
GribStatus gribUnpackDouble(AccessorPtr ptr, double& value);
GribStatus gribUnpackFloat(AccessorPtr ptr, std::vector<float>& values);
GribStatus gribUnpackFloat(AccessorPtr ptr, float& value);
GribStatus gribUnpackDoubleElement(AccessorPtr ptr, size_t i, std::vector<double>& values);
GribStatus gribUnpackFloatElement(AccessorPtr ptr, size_t i, std::vector<float>& values);
GribStatus gribUnpackDoubleElementSet(AccessorPtr ptr, const std::vector<size_t>& index_array, std::vector<double>& valArray);
GribStatus gribUnpackFloatElementSet(AccessorPtr ptr, const std::vector<size_t>& index_array, std::vector<float>& valArray);
GribStatus gribUnpackString(AccessorPtr ptr, std::string& value);
GribStatus gribUnpackStringArray(AccessorPtr ptr, std::vector<std::string>& values);
GribStatus gribUnpackLong(AccessorPtr ptr, std::vector<long>& values);
GribStatus gribUnpackLong(AccessorPtr ptr, long& value);

GribStatus gribAccessorsListUnpackLong(std::vector<AccessorPtr> ptrList, std::vector<long>& values);
GribStatus gribAccessorsListUnpackDouble(std::vector<AccessorPtr> ptrList, std::vector<double>& values);
GribStatus gribAccessorsListUnpackFloat(std::vector<AccessorPtr> ptrList, std::vector<float>& values);
GribStatus gribAccessorsListUnpackString(std::vector<AccessorPtr> ptrList, std::vector<std::string>& values);
GribStatus gribAccessorsListValueCount(std::vector<AccessorPtr> ptrList, size_t& count);
std::vector<AccessorPtr> gribAccessorsListCreate();
void gribAccessorsListPush(std::vector<AccessorPtr> ptrList, AccessorPtr ptr, int rank);
void gribAccessorsListDelete(std::vector<AccessorPtr> ptrList);

long gribAccessorGetNativeType(AccessorPtr ptr);
long gribGetNextPositionOffset(AccessorPtr ptr);
long gribStringLength(AccessorPtr ptr);
long gribByteOffset(AccessorPtr ptr);
long gribByteCount(AccessorPtr ptr);

GribStatus gribValueCount(AccessorPtr ptr, std::vector<long>& count);
GribStatus gribAccessorNotifyChange(AccessorPtr ptr, AccessorPtr changed);

void gribInitAccessor(AccessorPtr ptr, const long len, AccessorInitData args);
void gribAccessorDelete(AccessorPtr ptr);
void gribUpdateSize(AccessorPtr ptr, size_t len);
void gribResize(AccessorPtr ptr, size_t newSize);
size_t gribPreferredSize(AccessorPtr ptr, int fromHandle);

GribStatus gribNearestSmallerValue(AccessorPtr ptr, double val, double& nearest);
GribStatus gribCompareAccessors(AccessorPtr ptr1, AccessorPtr ptr2, int compareFlags);
GribStatus gribAccessorAddAttribute(AccessorPtr ptr, AccessorPtr attr, int nestIfClash);
GribStatus gribAccessorReplaceAttribute(AccessorPtr ptr, AccessorPtr attr);
GribStatus gribAccessorDeleteAttribute(AccessorPtr ptr, const std::string name);
GribStatus gribAccessorHasAttributes(AccessorPtr ptr);

AccessorPtr gribAccessorClone(AccessorPtr ptr, int& err);
AccessorPtr gribAccessorGetAttribute(AccessorPtr ptr, const std::string name);
std::vector<AccessorPtr> gribAccessorsListLast(std::vector<AccessorPtr> ptrList);
}
