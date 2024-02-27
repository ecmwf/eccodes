/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "LongData.h"
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

// Globals - END


LongData::LongData(const AccessorInitData& initData) :
    AccessorData(initData)
{
    //std::cout << "LongData::LongData()" << std::endl;
    name_ = "LongData";
}

GribType LongData::nativeType() const
{
    return GribType::LONG;
}

void LongData::dump() const
{
    // C++ implementation not yet available.
    // Current C++ conversion provided below (disabled, for reference only):

#if 0
gribDumpLong(NULL);
#endif  // 0

    return AccessorData::dump();
}

GribStatus LongData::unpack(std::string& stringValue) const
{
    GribStatus err = GribStatus{ 0 };
    long val       = 0;
    size_t l       = 1;
    std::string repres(1024, {});
    std::string format = "%ld";
    // [Removed invalid variable] grib_handle* h = grib_handle_of_accessor(a);
    err = unpack(val);
    static_cast<void>(err);
    if ((val == GRIB_MISSING_LONG) && ((flags_ & toInt(GribAccessorFlag::CAN_BE_MISSING)) != 0)) {
        fmtString(repres, repres.size(), "MISSING");
    }
    l = repres[0];
    if (l > stringValue.size()) {
        gribLog(GRIB_LOG_ERROR, "grib_accessor_long : unpack_string : Buffer too small for %s ", name_);
        stringValue.resize(l);
        return GribStatus::BUFFER_TOO_SMALL;
    }
    stringValue.resize(l);
    stringValue = repres;
    return GribStatus::SUCCESS;
}

GribStatus LongData::packMissing()
{
    size_t len = 1;
    long value = GRIB_MISSING_LONG;
    if (flags_ & toInt(GribAccessorFlag::CAN_BE_MISSING))
        return pack(value);
    return GribStatus::VALUE_CANNOT_BE_MISSING;
}

GribStatus LongData::unpack(std::vector<double>& doubleValues) const
{
    size_t rlen              = 0;
    long count               = 0;
    unsigned long i          = 0;
    std::vector<long> values = {};
    long oneval              = 0;
    GribStatus ret           = GribStatus::SUCCESS;
    ret                      = valueCount(count);
    if (GribStatusSuccess(ret))
        return ret;
    rlen = count;
    if (doubleValues.size() < rlen) {
        gribLog(GRIB_LOG_ERROR, "Wrong size for %s, it contains %lu values", name_, rlen);
        doubleValues.clear();
        return GribStatus::ARRAY_TOO_SMALL;
    }
    if (rlen == 1) {
        ret = unpack(oneval);
        if (ret != GribStatus::SUCCESS)
            return ret;
        doubleValues[0] = oneval;
        doubleValues.resize(1);
        return GribStatus::SUCCESS;
    }
    values.resize(rlen * sizeof(long));
    if (values.empty())
        return GribStatus::OUT_OF_MEMORY;
    ret = unpack(values);
    if (ret != GribStatus::SUCCESS) {
        // [Removed call to grib_context_free] grib_context_free(a->context, values);
        return ret;
    }
    for (i = 0; i < rlen; i++)
        doubleValues[i] = values[i];
    // [Removed call to grib_context_free] grib_context_free(a->context, values);
    doubleValues.resize(rlen);
    return GribStatus::SUCCESS;
}

bool LongData::compare(const AccessorData& rhs) const
{
    // C++ implementation not yet available.
    // Current C++ conversion provided below (disabled, for reference only):

#if 0
int retval = 0;
std::vector<long> aval = {};
std::vector<long> bval = {};
long count = 0;
size_t alen = 0;
size_t blen = 0;
GribStatus err = GribStatus{0};
err = valueCount(count);
if (GribStatusSuccess(err))
return static_cast<bool>(err);
alen = count;
err = gribValueCount(rhs, count);
if (GribStatusSuccess(err))
return static_cast<bool>(err);
blen = count;
if (alen != blen)
return GribStatus::COUNT_MISMATCH;
aval.resize(alen * sizeof(long));
bval.resize(b->context,blen * sizeof(long));
unpack(aval);
gribUnpackLong(rhs, bval);
retval = GribStatus::SUCCESS;
for(size_t i = 0;i < alen && retval == GribStatus::SUCCESS;++i)
{
if (aval.size() != bval[i])
retval = GribStatus::LONG_VALUE_MISMATCH;
}
// [Removed call to grib_context_free] grib_context_free(a->context, aval);
// [Removed call to grib_context_free] grib_context_free(b->context, bval);
return static_cast<bool>(retval);
#endif  // 0

    return AccessorData::compare(rhs);
}

GribStatus LongData::pack(const std::string& stringValue)
{
    long v = 0;
    if (strcmpNoCase(stringValue, "missing")) {
        return packMissing();
    }
    if (stringToLong(stringValue, v, 1) != GribStatus::SUCCESS) {
        gribLog(GRIB_LOG_ERROR, "Trying to pack \"%s\" as long. String cannot be converted to an integer", stringValue);
        return GribStatus::WRONG_TYPE;
    }
    return pack(v);
}


}  // namespace eccodes::accessor
