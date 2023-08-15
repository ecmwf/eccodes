#include "AccessorData.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

// Default Implementation for AccessorData virtual functions

void AccessorData::dumpImpl(grib_dumper const& dumper) const
{
    // TO DO!

/*
    switch (nativeTypeImpl()) {
        case GRIB_TYPE_STRING:
            grib_dump_string(&dumper, a, NULL);
            break;
        case GRIB_TYPE_DOUBLE:
            grib_dump_double(&dumper, a, NULL);
            break;
        case GRIB_TYPE_LONG:
            grib_dump_long(&dumper, a, NULL);
            break;
        default:
            grib_dump_bytes(&dumper, a, NULL);
    }
*/
}

std::size_t AccessorData::stringLengthImpl() const
{
    return 1024;
}

int AccessorData::valueCountImpl(long& count) const
{
    count =1;
    return GRIB_SUCCESS;
}

long AccessorData::byteCountImpl() const
{
    return length_;
}

long AccessorData::byteOffsetImpl() const
{
    return offset_;
}

int AccessorData::nativeTypeImpl() const
{
    return GRIB_TYPE_UNDEFINED;
}

void AccessorData::updateSizeImpl(std::size_t s)
{
    Assert(0);
}

std::size_t AccessorData::preferredSizeImpl(int fromHandle) const
{
    return length_;
}

void AccessorData::resizeImpl(size_t newSize)
{
    Assert(0);
}

int AccessorData::nearestSmallerValueImpl(double val, double& nearest) const
{
    Assert(0);
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::compareImpl(AccessorData const& rhs)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::packMissingImpl()
{
    Assert(0);
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::isMissingImpl() const
{
    if (flags_ & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        if (!vvalue_) {
//            grib_context_log(a->context, GRIB_LOG_ERROR, "%s internal error (flags=0x%lX)", a->name, a->flags);
//            Assert(!"grib_accessor_class_gen::is_missing(): a->vvalue == NULL");
            Assert(0);
            return 0;
        }
        return vvalue_->missing;
    }
    Assert(length_ >= 0);

    // TO DO
    unsigned char* v = nullptr; //grib_handle_of_accessor(a)->buffer->data + offset_;
    Assert(0);

    int isMissing{1};
    unsigned char ones{0xff};
    
    for (int i{0}; i < length_; i++) {
        if (*v != ones) {
            isMissing = 0;
            break;
        }
        v++;
    }

    return isMissing;
}

int AccessorData::packImpl(std::vector<long> const& values)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackImpl(std::vector<long> &values) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::packImpl(std::vector<double> const& values)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackImpl(std::vector<double> &values) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::packImpl(std::vector<float> const& values)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackImpl(std::vector<float> &values) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::packImpl(std::vector<std::string> const& values)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackImpl(std::vector<std::string> &values) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::packImpl(std::vector<stringArray> const& values)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackImpl(std::vector<stringArray> &values) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::packImpl(std::vector<std::byte> const& values)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackImpl(std::vector<std::byte> &values) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::packImpl(grib_expression const& expression)
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackElementImpl(std::size_t index, double& val) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackElementImpl(std::size_t index, float& val) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackElementSetImpl(std::vector<std::size_t> const& indexArray, std::vector<double> &valArray) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackElementSetImpl(std::vector<std::size_t> const& indexArray, std::vector<float> &valArray) const
{
    return GRIB_NOT_IMPLEMENTED;
}

int AccessorData::unpackSubarrayImpl(std::vector<double> &values, std::size_t start) const
{
    return GRIB_NOT_IMPLEMENTED;
}


}
