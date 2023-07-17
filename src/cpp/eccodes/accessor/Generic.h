#ifndef _GENERIC_H_20230705_
#define _GENERIC_H_20230705_

#include "grib_api_internal.h"

namespace eccodes::accessor {

struct Generic
{
    // mutable during initial conversion from C - to be addressed later...
    mutable grib_accessor att_{};

    Generic(grib_section* p, grib_action* creator, const long len, grib_arguments* arg);
    virtual ~Generic();

    operator grib_accessor*() const { return &att_; }

    // Prevent copies and moves
    Generic(Generic const&) = delete;
    Generic& operator=(Generic const&) = delete;
    Generic(Generic&&) = delete;
    Generic& operator=(Generic&&) = delete;
};

// Templated versions of the Accessor functions that provide a "fallback" version if not specialised

template<typename ACCESSOR_TYPE>
void dump(ACCESSOR_TYPE const& accessor, grib_dumper* dumper)
{
    int type = grib_accessor_get_native_type(accessor);

    switch (type) {
        case GRIB_TYPE_STRING:
            grib_dump_string(dumper, accessor, NULL);
            break;
        case GRIB_TYPE_DOUBLE:
            grib_dump_double(dumper, accessor, NULL);
            break;
        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, accessor, NULL);
            break;
        default:
            grib_dump_bytes(dumper, accessor, NULL);
    }
}

template<typename ACCESSOR_TYPE>
long nextOffset(ACCESSOR_TYPE const& accessor)
{
    return accessor.att_.offset + accessor.att_.length;
}

template<typename ACCESSOR_TYPE>
size_t stringLength(ACCESSOR_TYPE const& accessor)
{
    return 1024;
}

template<typename ACCESSOR_TYPE>
int valueCount(ACCESSOR_TYPE const& accessor, long* count)
{
    *count = 1;
    return 0;
}

template<typename ACCESSOR_TYPE>
long byteCount(ACCESSOR_TYPE const& accessor)
{
    return accessor.att_.length;
}

template<typename ACCESSOR_TYPE>
long byteOffset(ACCESSOR_TYPE const& accessor)
{
    return accessor.att_.offset;
}

template<typename ACCESSOR_TYPE>
int nativeType(ACCESSOR_TYPE const& accessor)
{
    grib_context_log(accessor.att_.context, GRIB_LOG_ERROR,
                     "Accessor %s must implement 'get_native_type'", accessor.att_.name);
    return GRIB_TYPE_UNDEFINED;
}

template<typename ACCESSOR_TYPE>
grib_section* subSection(ACCESSOR_TYPE const& accessor)
{
    return NULL;
}

template<typename ACCESSOR_TYPE>
int packMissing(ACCESSOR_TYPE& accessor)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int isMissing(ACCESSOR_TYPE const& accessor)
{
    int i              = 0;
    int is_missing     = 1;
    unsigned char ones = 0xff;
    unsigned char* v   = NULL;

    if (accessor.att_.flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        if (accessor.att_.vvalue == NULL) {
            grib_context_log(accessor.att_.context, GRIB_LOG_ERROR, "%s internal error (flags=0x%lX)", accessor.att_.name, accessor.att_.flags);
            Assert(!"grib_accessor_class_gen::is_missing(): accessor.att_.vvalue == NULL");
            return 0;
        }
        return accessor.att_.vvalue->missing;
    }
    Assert(accessor.att_.length >= 0);

    v = grib_handle_of_accessor(accessor)->buffer->data + accessor.att_.offset;

    for (i = 0; i < accessor.att_.length; i++) {
        if (*v != ones) {
            is_missing = 0;
            break;
        }
        v++;
    }

    return is_missing;
}

template<typename ACCESSOR_TYPE>
int packLong(ACCESSOR_TYPE& accessor, const long* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackLong(ACCESSOR_TYPE const& accessor, long* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int packDouble(ACCESSOR_TYPE& accessor, const double* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int packFloat(ACCESSOR_TYPE& accessor, const float* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackDouble(ACCESSOR_TYPE const& accessor, double* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackFloat(ACCESSOR_TYPE const& accessor, float* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int packString(ACCESSOR_TYPE& accessor, const char* v, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackString(ACCESSOR_TYPE const& accessor, char* v, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int packStringArray(ACCESSOR_TYPE& accessor, const char** v, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackStringArray(ACCESSOR_TYPE const& accessor, char** v, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int packBytes(ACCESSOR_TYPE& accessor, const unsigned char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackBytes(ACCESSOR_TYPE const& accessor, unsigned char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int packExpression(ACCESSOR_TYPE& accessor, grib_expression* e)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int notifyChange(ACCESSOR_TYPE& accessor, grib_accessor* observed)
{
    /* Default behaviour is to notify creator */
    return grib_action_notify_change(accessor.att_.creator, accessor, observed);
}

template<typename ACCESSOR_TYPE>
void updateSize(ACCESSOR_TYPE& accessor, size_t s)
{
    grib_context_log(accessor.att_.context, GRIB_LOG_ERROR,
                     "Accessor %s must implement 'update_size'", accessor.att_.name);
    Assert(0 == 1);
}

template<typename ACCESSOR_TYPE>
size_t preferredSize(ACCESSOR_TYPE const& accessor, int from_handle)
{
    return accessor.att_.length;
}

template<typename ACCESSOR_TYPE>
void resize(ACCESSOR_TYPE const& accessor, size_t new_size)
{
    return;
}

template<typename ACCESSOR_TYPE>
int nearestSmallerValue(ACCESSOR_TYPE const& accessor, double val, double* nearest)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
grib_accessor* next(ACCESSOR_TYPE const& accessor, int mod)
{
    grib_accessor* next = NULL;
    if (accessor.att_.next) {
        next = accessor.att_.next;
    }
    else {
        // TODO - Need to convert parent->owner to Accessor)
        if (accessor.att_.parent->owner)
            next = accessor.att_.parent->owner->cclass->next(accessor.att_.parent->owner, 0);
    }
    return next;
}

template<typename ACCESSOR_TYPE>
int compare(ACCESSOR_TYPE const& accessor, grib_accessor* b)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackDoubleElement(ACCESSOR_TYPE const& accessor, size_t i, double* val)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackFloatElement(ACCESSOR_TYPE const& accessor, size_t i, float* val)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackDoubleElementSet(ACCESSOR_TYPE const& accessor, const size_t* index_array, size_t len, double* val_array)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackFloatElementSet(ACCESSOR_TYPE const& accessor, const size_t* index_array, size_t len, float* val_array)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int unpackDoubleSubarray(ACCESSOR_TYPE const& accessor, double* val, size_t start, size_t len)
{
    return GRIB_NOT_IMPLEMENTED;
}

template<typename ACCESSOR_TYPE>
int clear(ACCESSOR_TYPE const& accessor)
{
    unsigned char* buf = grib_handle_of_accessor(accessor)->buffer->data;
    long length        = grib_byte_count(accessor);
    long offset        = grib_byte_offset(accessor);

    memset(buf + offset, 0, length);

    return GRIB_SUCCESS;
}

template<typename ACCESSOR_TYPE>
grib_accessor* makeClone(ACCESSOR_TYPE const& accessor, grib_section* s, int* err)
{
    *err = GRIB_NOT_IMPLEMENTED; 
    return NULL;
}


}
#endif // _GENERIC_H_20230705_
