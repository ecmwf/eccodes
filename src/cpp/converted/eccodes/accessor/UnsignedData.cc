/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#include "UnsignedData.h"
#include "Accessor.h"
#include "AccessorFactory.h"
#include "AccessorUtils/ConversionHelper.h"
#include "AccessorUtils/ContainerHelper.h"
#include "AccessorUtils/AccessorException.h"
#include "GribCpp/GribCppIncludes.h"
#include "GribStub/GribStubIncludes.h"


namespace eccodes::accessor
{

// Globals - START
const std::vector<unsigned long> ones = { 0, 0xff, 0xffff, 0xffffff, 0xffffffff };
unsigned long allOnes                 = -1;
bool valueIsMissing(long val);
// Globals - END


bool valueIsMissing(long val)
{
    return val == GRIB_MISSING_LONG || val == allOnes;
}

GribStatus UnsignedData::packLongUnsignedHelper(const std::vector<long>& val, int check)
{
    GribStatus ret         = GribStatus::SUCCESS;
    long off               = 0;
    long rlen              = 0;
    size_t buflen          = 0;
    AccessorDataBuffer buf = {};
    unsigned long i        = 0;
    unsigned long missing  = 0;
    GribStatus err         = valueCount(rlen);

    if (!GribStatusSuccess(err))
        return err;

    if (flags_ & toInt(GribAccessorFlag::CAN_BE_MISSING)) {
        Assert(nbytes_ <= 4);
        missing = ones[nbytes_];
    }

    if (flags_ & toInt(GribAccessorFlag::TRANSIENT)) {
        vvalue_->lval = val[0];
        if (missing && val.size() == GRIB_MISSING_LONG)
            vvalue_->lval = 1;
        return GribStatus::SUCCESS;
    }

    if (val.size() < 1) {
        gribLog(GRIB_LOG_ERROR, "Wrong size for %s it contains %zu values ", name_, val.size());
        return GribStatus::ARRAY_TOO_SMALL;
    }

    if (rlen == 1) {
        long v = val[0];
        if (missing)
            if (v == GRIB_MISSING_LONG)
                v = missing;
        if (check) {
            if (val[0] < 0) {
                gribLog(GRIB_LOG_ERROR, "Key \"%s\": Trying to encode a negative value of %ld for key of type unsigned", name_, val[0]);
                return GribStatus::ENCODING_ERROR;
            }
            if (!valueIsMissing(v)) {
                const long nbits = nbytes_ * 8;
                if (nbits < 33) {
                    unsigned long maxval = (1UL << nbits) - 1;
                    if (maxval > 0 && v > maxval) {
                        gribLog(GRIB_LOG_ERROR, "Key \"%s\": Trying to encode value of %ld but the maximum allowable value is %lu (number of bits=%ld)", name_, v, maxval, nbits);
                        return GribStatus::ENCODING_ERROR;
                    }
                }
            }
        }
        off = offset_ * 8;
        ret = gribEncodeUnsignedLong(buffer_.data(), v, off, nbytes_ * 8);
        //if (ret == GribStatus::SUCCESS)
            // [Removing - conversion requires mutable operation on const type] val.resize(1);

        if (val.size() > 1)
            gribLog(GRIB_LOG_WARNING, "grib_accessor_unsigned : Trying to pack %d values in a scalar %s, packing first value", val.size(), name_);
        // [Removing - conversion requires mutable operation on const type] val.resize(1);
        return ret;
    }

    buflen = val[0] * nbytes_;
    buf.resize(buflen);
    for (i = 0; i < val.size(); i++)
        gribEncodeUnsignedLong(buf, val[i], off, nbytes_ * 8);
    //ret = gribSetLong(AccessorName(std::get<std::string>(initData.args[0].second)), val[0]);
    std::string parentName = std::get<std::string>(args_[0].second);

    ret = gribSetLong(AccessorName(parentName), val[0]);
    if (ret == GribStatus::SUCCESS)
        gribBufferReplace(buf, buflen, 1, 1);
    // [Removed call to grib_context_free] grib_context_free(a->context, buf);
    return ret;
}


UnsignedData::UnsignedData(const AccessorInitData& initData) :
    LongData(initData)
{
    name_ = "unsigned";
    args_    = initData.args;
    nbytes_ = initData.length;
    if (flags_ & toInt(GribAccessorFlag::TRANSIENT)) {
        length_ = 0;
        if (!vvalue_)
            vvalue_ = std::make_shared<GribVirtualValue>();
        vvalue_->type = GribType::LONG;
        vvalue_->length = initData.length;
    }
    else {
        long count = 0;
        valueCount(count);
        length_ = count * nbytes_;
        vvalue_ = nullptr;
    }
}

void UnsignedData::dump() const
{
    // C++ implementation not yet available.
    // Current C++ conversion provided below (disabled, for reference only):

#if 0
long rlen = 0;
valueCount(rlen);
if (rlen == 1)
gribDumpLong(NULL);
#endif  // 0

    return LongData::dump();
}

GribStatus UnsignedData::unpack(std::vector<long>& values) const
{
    long rlen             = 0;
    unsigned long i       = 0;
    unsigned long missing = 0;
    long count            = 0;
    GribStatus err        = GribStatus{ 0 };

    unsigned char* data = currentBuffer().data();
    long bitPos = byteOffset() * 8;


    err = valueCount(count);
    if (!GribStatusSuccess(err))
        return err;

    values.resize(count);

    rlen = count;
    if (values.size() < rlen) {
        gribLog(GRIB_LOG_ERROR, "Wrong size (%ld) for %s, it contains %ld values", values.size(), name_, rlen);
        values.clear();
        return GribStatus::ARRAY_TOO_SMALL;
    }
    if (flags_ & toInt(GribAccessorFlag::TRANSIENT)) {
        values.resize(1);
        values[0] = vvalue_->lval;
        return GribStatus::SUCCESS;
    }
    if (flags_ & toInt(GribAccessorFlag::CAN_BE_MISSING)) {
        Assert(nbytes_ <= 4);
        missing = ones[0];
    }
    for (i = 0; i < rlen; i++) {
        values[i] = static_cast<long>(gribDecodeUnsignedLong(data, bitPos, nbytes_ * 8));
        if (missing)
            if (values.size() == missing)
                values[i] = GRIB_MISSING_LONG;
    }
    values.resize(rlen);
    return GribStatus::SUCCESS;
}

GribStatus UnsignedData::pack(const std::vector<long>& longValues)
{
    return packLongUnsignedHelper(longValues, 1);
}

//long UnsignedData::byteCount() const
//{
//    return length_;
//}

GribStatus UnsignedData::valueCount(long& count) const
{
    if (!args_.empty()) {
        count = 1;
        return GribStatus::SUCCESS;
    }

    return LongData::valueCount(count);
}

//long UnsignedData::byteOffset() const
//{
//    return offset_;
//}

void UnsignedData::updateSize(std::size_t s)
{
    length_ = s;
}

bool UnsignedData::isMissing() const
{
    int i                = 0;
    unsigned char ff     = 0xff;
    unsigned long offset = offset_;
    if (length_ == 0) {
        Assert(vvalue_ != nullptr);
        return static_cast<bool>(vvalue_);
    }
    for (i = 0; i < length_; i++) {
        if (buffer_.data()[offset] != ff) {
            return 0;
        }
        offset++;
    }
    return 1;
}

UnsignedData::~UnsignedData(){}

AccessorBuilder<UnsignedData> unsignedDataBuilder(AccessorType("unsigned"));

}  // namespace eccodes::accessor
