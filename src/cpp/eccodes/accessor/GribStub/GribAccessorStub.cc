#include "GribAccessorStub.h"
#include "Accessor.h"

#include <cassert>

namespace eccodes::accessor {

GribStatus gribInlineStrcmp(const std::string a, const std::string b)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackMissing(AccessorPtr ptr)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackZero(AccessorPtr ptr)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribIsMissingInternal(AccessorPtr ptr)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackDouble(AccessorPtr ptr, const std::vector<double>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackDouble(AccessorPtr ptr, const double value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackFloat(AccessorPtr ptr, const std::vector<float>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackFloat(AccessorPtr ptr, const float value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackExpression(AccessorPtr ptr, GribExpressionPtr e)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackString(AccessorPtr ptr, const std::string value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackStringArray(AccessorPtr ptr, const std::vector<std::string>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackLong(AccessorPtr ptr, const std::vector<long>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackLong(AccessorPtr ptr, const long value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribPackBytes(AccessorPtr ptr, const std::vector<unsigned char>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackBytes(AccessorPtr ptr, std::vector<unsigned char>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackDoubleSubarray(AccessorPtr ptr, std::vector<double>& v, size_t start)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackDouble(AccessorPtr ptr, std::vector<double>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackDouble(AccessorPtr ptr, double& value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackFloat(AccessorPtr ptr, std::vector<float>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackFloat(AccessorPtr ptr, float& value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackDoubleElement(AccessorPtr ptr, size_t i, std::vector<double>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackFloatElement(AccessorPtr ptr, size_t i, std::vector<float>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackDoubleElementSet(AccessorPtr ptr, const std::vector<size_t>& index_array, std::vector<double>& valArray)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackFloatElementSet(AccessorPtr ptr, const std::vector<size_t>& index_array, std::vector<float>& valArray)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackString(AccessorPtr ptr, std::string& value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackStringArray(AccessorPtr ptr, std::vector<std::string>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackLong(AccessorPtr ptr, std::vector<long>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribUnpackLong(AccessorPtr ptr, long& value)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}


GribStatus gribAccessorsListUnpackLong(std::vector<AccessorPtr> ptrList, std::vector<long>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorsListUnpackDouble(std::vector<AccessorPtr> ptrList, std::vector<double>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorsListUnpackFloat(std::vector<AccessorPtr> ptrList, std::vector<float>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorsListUnpackString(std::vector<AccessorPtr> ptrList, std::vector<std::string>& values)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorsListValueCount(std::vector<AccessorPtr> ptrList, size_t& count)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

std::vector<AccessorPtr> gribAccessorsListCreate()
{
    assert(false); // TO DO

    return {};
}

void gribAccessorsListPush(std::vector<AccessorPtr> ptrList, AccessorPtr ptr, int rank)
{
    assert(false); // TO DO
}

void gribAccessorsListDelete(std::vector<AccessorPtr> ptrList)
{
    assert(false); // TO DO
}


long gribAccessorGetNativeType(AccessorPtr ptr)
{
    assert(false); // TO DO

    return 0;
}

long gribGetNextPositionOffset(AccessorPtr ptr)
{
    assert(false); // TO DO

    return 0;
}

long gribStringLength(AccessorPtr ptr)
{
    assert(false); // TO DO

    return 0;
}

long gribByteOffset(AccessorPtr ptr)
{
    assert(false); // TO DO

    return 0;
}

long gribByteCount(AccessorPtr ptr)
{
    assert(false); // TO DO

    return 0;
}


GribStatus gribValueCount(AccessorPtr ptr, std::vector<long>& count)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorNotifyChange(AccessorPtr ptr, AccessorPtr changed)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}


void gribInitAccessor(AccessorPtr ptr, const long len, AccessorInitData args)
{
    assert(false); // TO DO
}

void gribAccessorDelete(AccessorPtr ptr)
{
    assert(false); // TO DO
}

void gribUpdateSize(AccessorPtr ptr, size_t len)
{
    assert(false); // TO DO
}

void gribResize(AccessorPtr ptr, size_t newSize)
{
    assert(false); // TO DO
}

size_t gribPreferredSize(AccessorPtr ptr, int fromHandle)
{
    assert(false); // TO DO

    return 0;
}


GribStatus gribNearestSmallerValue(AccessorPtr ptr, double val, double& nearest)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribCompareAccessors(AccessorPtr ptr1, AccessorPtr ptr2, int compareFlags)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorAddAttribute(AccessorPtr ptr, AccessorPtr attr, int nestIfClash)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorReplaceAttribute(AccessorPtr ptr, AccessorPtr attr)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorDeleteAttribute(AccessorPtr ptr, const std::string name)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribAccessorHasAttributes(AccessorPtr ptr)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

AccessorPtr gribAccessorClone(AccessorPtr ptr, int& err)
{
    assert(false); // TO DO

    return nullptr;
}

AccessorPtr gribAccessorGetAttribute(AccessorPtr ptr, const std::string name)
{
    assert(false); // TO DO

    return nullptr;
}

std::vector<AccessorPtr> gribAccessorsListLast(std::vector<AccessorPtr> ptrList)
{
    assert(false); // TO DO

    return {};
}

}
