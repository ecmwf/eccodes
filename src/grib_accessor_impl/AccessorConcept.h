#ifndef _ACCESSOR_CONCEPT_H_20230711_
#define _ACCESSOR_CONCEPT_H_20230711_

#include <stddef.h> // size_t

struct grib_accessor;
struct grib_dumper;
struct grib_section;
struct grib_expression;

namespace eccodes {

// Define all APIs available to an accessor

struct AccessorConcept
{
    virtual ~AccessorConcept() = default;

    virtual operator grib_accessor*() = 0;
    virtual void accessorDump(grib_dumper* dumper) = 0;
    virtual long accessorNextOffset() = 0;
    virtual size_t accessorStringLength() = 0;
    virtual int accessorValueCount(long* count) = 0;
    virtual long accessorByteCount() = 0;
    virtual long accessorByteOffset() = 0;
    virtual int accessorNativeType() = 0;
    virtual grib_section* accessorSubSection() = 0;
    virtual int accessorPackMissing() = 0;
    virtual int accessorIsMissing() = 0;

    virtual int accessorPackLong(const long* val, size_t* len) = 0;
    virtual int accessorPackDouble(const double* val, size_t* len) = 0;
    virtual int accessorPackFloat(const float* val, size_t* len) = 0;
    virtual int accessorPackString(const char* v, size_t* len) = 0;
    virtual int accessorPackStringArray(const char** v, size_t* len) = 0;
    virtual int accessorPackBytes(const unsigned char* val, size_t* len) = 0;
    virtual int accessorPackExpression(grib_expression* e) = 0;
    
    virtual int accessorUnpackLong(long* val, size_t* len) = 0;
    virtual int accessorUnpackDouble(double* val, size_t* len) = 0;
    virtual int accessorUnpackFloat(float* val, size_t* len) = 0;
    virtual int accessorUnpackString(char* v, size_t* len) = 0;
    virtual int accessorUnpackStringArray(char** v, size_t* len) = 0;
    virtual int accessorUnpackBytes(unsigned char* val, size_t* len) = 0;
    virtual int accessorUnpackDoubleElement(size_t i, double* val) = 0;
    virtual int accessorUnpackFloatElement(size_t i, float* val) = 0;
    virtual int accessorUnpackDoubleElementSet(const size_t* index_array, size_t len, double* val_array) = 0;
    virtual int accessorUnpackFloatElementSet(const size_t* index_array, size_t len, float* val_array) = 0;
    virtual int accessorUnpackDoubleSubarray(double* val, size_t start, size_t len) = 0;

    virtual int accessorNotifyChange(grib_accessor* observed) = 0;
    virtual void accessorUpdateSize(size_t s) = 0;
    virtual size_t accessorPreferredSize(int from_handle) = 0;
    virtual void accessorResize(size_t new_size) = 0;
    virtual int accessorNearestSmallerValue(double val, double* nearest) = 0;
    virtual grib_accessor* accessorNext(int mod) = 0;
    virtual int accessorCompare(grib_accessor* b) = 0;
    virtual int accessorClear() = 0;
    virtual grib_accessor* accessorMakeClone(grib_section* s, int* err) = 0; 
};

}

#endif // _ACCESSOR_CONCEPT_H_20230711_
