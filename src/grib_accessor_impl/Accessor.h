#ifndef _ACCESSOR_H_20230630_
#define _ACCESSOR_H_20230630_

#include <memory>

struct grib_accessor;

#include "AccessorConcept.h"
#include "ProjString.h"

namespace eccodes {

// Helper for template type deduction
template<typename ACCESSOR_TYPE>
struct AccessorType{};

class Accessor
{
private:
    template<typename ACCESSOR_TYPE>
    struct AccessorModel : AccessorConcept
    {
        AccessorModel(ACCESSOR_TYPE const& accessor) : accessor_{accessor} {}
        AccessorModel(ACCESSOR_TYPE && accessor) : accessor_{std::move(accessor)} {}

        AccessorModel(grib_section* p, grib_action* creator, const long len, grib_arguments* arg) : 
            accessor_{p, creator, len, arg} {}

        operator grib_accessor*() override { return accessor_; }
        
        void accessorDump(grib_dumper* dumper) final { return dump(accessor_, dumper); }
        long accessorNextOffset() final { return nextOffset(accessor_); }
        size_t accessorStringLength() final { return stringLength(accessor_); }
        int accessorValueCount(long* count) final { return valueCount(accessor_, count); }
        long accessorByteCount() final { return byteCount(accessor_); }
        long accessorByteOffset() final { return byteOffset(accessor_); }
        int accessorNativeType() final { return nativeType(accessor_); }
        grib_section* accessorSubSection() final { return subSection(accessor_); }
        int accessorPackMissing() final { return packMissing(accessor_); }
        int accessorIsMissing() final { return isMissing(accessor_); }

        int accessorPackLong(const long* val, size_t* len) final { return packLong(accessor_, val, len); }
        int accessorPackDouble(const double* val, size_t* len) final { return packDouble(accessor_, val, len); }
        int accessorPackFloat(const float* val, size_t* len) final { return packFloat(accessor_, val, len); }
        int accessorPackString(const char* v, size_t* len) final { return packString(accessor_, v, len); }
        int accessorPackStringArray(const char** v, size_t* len) final { return packStringArray(accessor_, v, len); }
        int accessorPackBytes(const unsigned char* val, size_t* len) final { return packBytes(accessor_, val, len); }
        int accessorPackExpression(grib_expression* e) final { return packExpression(accessor_, e); }
        
        int accessorUnpackLong(long* val, size_t* len) final { return unpackLong(accessor_, val, len); }
        int accessorUnpackDouble(double* val, size_t* len) final { return unpackDouble(accessor_, val, len); }
        int accessorUnpackFloat(float* val, size_t* len) final { return unpackFloat(accessor_, val, len); }
        int accessorUnpackString(char* v, size_t* len) final { return unpackString(accessor_, v, len); }
        int accessorUnpackStringArray(char** v, size_t* len) final { return unpackStringArray(accessor_, v, len); }
        int accessorUnpackBytes(unsigned char* val, size_t* len) final { return unpackBytes(accessor_, val, len); }
        int accessorUnpackDoubleElement(size_t i, double* val) final { return unpackDoubleElement(accessor_, i, val); }
        int accessorUnpackFloatElement(size_t i, float* val) final { return unpackFloatElement(accessor_, i, val); }
        int accessorUnpackDoubleElementSet(const size_t* index_array, size_t len, double* val_array) final { return unpackDoubleElementSet(accessor_, index_array, len, val_array); }
        int accessorUnpackFloatElementSet(const size_t* index_array, size_t len, float* val_array) final { return unpackFloatElementSet(accessor_, index_array, len, val_array); }
        int accessorUnpackDoubleSubarray(double* val, size_t start, size_t len) final { return unpackDoubleSubarray(accessor_, val, start, len); }

        int accessorNotifyChange(grib_accessor* observed) final { return notifyChange(accessor_, observed); }
        void accessorUpdateSize(size_t s) final { return updateSize(accessor_, s); }
        size_t accessorPreferredSize(int from_handle) final { return preferredSize(accessor_, from_handle); }
        void accessorResize(size_t new_size) final { return resize(accessor_, new_size); }
        int accessorNearestSmallerValue(double val, double* nearest) final { return nearestSmallerValue(accessor_, val, nearest); }
        grib_accessor* accessorNext(int mod) final { return next(accessor_, mod); }
        int accessorCompare(grib_accessor* b) final { return compare(accessor_, b); }
        int accessorClear() final { return clear(accessor_); }
        grib_accessor* accessorMakeClone(grib_section* s, int* err) final { return makeClone(accessor_, s, err); } 

        ACCESSOR_TYPE accessor_;
    };

    friend grib_accessor* asGribAccessor(Accessor const& accessor);
    friend void accessorDump(Accessor const& accessor, grib_dumper* dumper);
    friend long accessorNextOffset(Accessor const& accessor);
    friend size_t accessorStringLength(Accessor const& accessor);
    friend int accessorValueCount(Accessor const& accessor, long* count);
    friend long accessorByteCount(Accessor const& accessor);
    friend long accessorByteOffset(Accessor const& accessor);
    friend int accessorNativeType(Accessor const& accessor);
    friend grib_section* accessorSubSection(Accessor const& accessor);
    friend int accessorPackMissing(Accessor const& accessor);
    friend int accessorIsMissing(Accessor const& accessor);

    friend int accessorPackLong(Accessor const& accessor, const long* val, size_t* len);
    friend int accessorPackDouble(Accessor const& accessor, const double* val, size_t* len);
    friend int accessorPackFloat(Accessor const& accessor, const float* val, size_t* len);
    friend int accessorPackString(Accessor const& accessor, const char* v, size_t* len);
    friend int accessorPackStringArray(Accessor const& accessor, const char** v, size_t* len);
    friend int accessorPackBytes(Accessor const& accessor, const unsigned char* val, size_t* len);
    friend int accessorPackExpression(Accessor const& accessor, grib_expression* e);
    
    friend int accessorUnpackLong(Accessor const& accessor, long* val, size_t* len);
    friend int accessorUnpackDouble(Accessor const& accessor, double* val, size_t* len);
    friend int accessorUnpackFloat(Accessor const& accessor, float* val, size_t* len);
    friend int accessorUnpackString(Accessor const& accessor, char* v, size_t* len);
    friend int accessorUnpackStringArray(Accessor const& accessor, char** v, size_t* len);
    friend int accessorUnpackBytes(Accessor const& accessor, unsigned char* val, size_t* len);
    friend int accessorUnpackDoubleElement(Accessor const& accessor, size_t i, double* val);
    friend int accessorUnpackFloatElement(Accessor const& accessor, size_t i, float* val);
    friend int accessorUnpackDoubleElementSet(Accessor const& accessor, const size_t* index_array, size_t len, double* val_array);
    friend int accessorUnpackFloatElementSet(Accessor const& accessor, const size_t* index_array, size_t len, float* val_array);
    friend int accessorUnpackDoubleSubarray(Accessor const& accessor, double* val, size_t start, size_t len);

    friend int accessorNotifyChange(Accessor const& accessor, grib_accessor* observed);
    friend void accessorUpdateSize(Accessor const& accessor, size_t s);
    friend size_t accessorPreferredSize(Accessor const& accessor, int from_handle);
    friend void accessorResize(Accessor const& accessor, size_t new_size);
    friend int accessorNearestSmallerValue(Accessor const& accessor, double val, double* nearest);
    friend grib_accessor* accessorNext(Accessor const& accessor, int mod);
    friend int accessorCompare(Accessor const& accessor, grib_accessor* b);
    friend int accessorClear(Accessor const& accessor);
    friend grib_accessor* accessorMakeClone(Accessor const& accessor, grib_section* s, int* err); 

    std::unique_ptr<AccessorConcept> pimpl;

public:
    template<typename ACCESSOR_TYPE>
    Accessor(ACCESSOR_TYPE const& accessor) : pimpl{ std::make_unique<AccessorModel<ACCESSOR_TYPE>>(accessor)} {}

    template<typename ACCESSOR_TYPE>
    Accessor(ACCESSOR_TYPE&& accessor) : pimpl{ std::make_unique<AccessorModel<ACCESSOR_TYPE>>(std::move(accessor))} {}

    // AccessorType<ACCESSOR_TYPE> is required to allow us to deduce the template type, as constructors only support
    // the <T> format when it is part of the class definition, i.e. Accessor<ProjString>(...) is invalid here...
    template<typename ACCESSOR_TYPE>
    Accessor(AccessorType<ACCESSOR_TYPE>, grib_section* p, grib_action* creator, const long len, grib_arguments* arg) : 
        pimpl{ std::make_unique<AccessorModel<ACCESSOR_TYPE>>(p, creator, len, arg)} {}

    Accessor(Accessor const&) = delete;
    Accessor& operator=(Accessor const&) = delete;
    Accessor(Accessor&&) = default;
    Accessor& operator=(Accessor&&) = default;
};

}

#endif // _ACCESSOR_H_20230630_
