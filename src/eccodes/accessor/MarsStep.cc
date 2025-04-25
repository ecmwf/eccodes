/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "MarsStep.h"

eccodes::accessor::MarsStep _grib_accessor_mars_step;
eccodes::Accessor* grib_accessor_mars_step = &_grib_accessor_mars_step;

namespace eccodes::accessor
{

void MarsStep::init(const long l, grib_arguments* c)
{
    Ascii::init(l, c);
    int n      = 0;
    stepRange_ = c->get_name(get_enclosing_handle(), n++);
    stepType_  = c->get_name(get_enclosing_handle(), n++);
}

int MarsStep::pack_string(const char* val, size_t* len)
{
    char stepType[100];
    size_t stepTypeLen = 100;
    char buf[100]      = {0,};
    int ret;
    grib_accessor* stepRangeAcc = grib_find_accessor(get_enclosing_handle(), stepRange_);

    if (!stepRangeAcc) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s not found", stepRange_);
        return GRIB_NOT_FOUND;
    }

    if ((ret = grib_get_string(get_enclosing_handle(), stepType_, stepType, &stepTypeLen)) != GRIB_SUCCESS)
        return ret;

    if (!strcmp(stepType, "instant"))
        snprintf(buf, sizeof(buf), "%s", val);
    else
        snprintf(buf, sizeof(buf), "0-%s", val);

    return stepRangeAcc->pack_string(buf, len);
}

int MarsStep::unpack_string(char* val, size_t* len)
{
    int ret       = 0;
    char buf[100] = {0,};
    char* p       = NULL;
    size_t buflen = 100;
    long step;
    grib_accessor* stepRangeAcc = grib_find_accessor(get_enclosing_handle(), stepRange_);

    if (!stepRangeAcc) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s: %s not found", class_name_, stepRange_);
        return GRIB_NOT_FOUND;
    }

    if ((ret = stepRangeAcc->unpack_string(buf, &buflen)) != GRIB_SUCCESS)
        return ret;

    if (*len < buflen) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         class_name_, name_, buflen, *len);
        *len = buflen;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(val, buf);
    step = strtol(buf, &p, 10);

    if (p != NULL && *p == '-' && step == 0)
        strcpy(val, ++p);

    *len = strlen(val);

    return ret;
}

int MarsStep::pack_long(const long* val, size_t* len)
{
    char buff[100] = {0,};
    size_t bufflen = 100;

    snprintf(buff, sizeof(buff), "%ld", *val);

    return pack_string(buff, &bufflen);
}

int MarsStep::unpack_long(long* val, size_t* len)
{
    grib_accessor* stepRangeAcc = grib_find_accessor(get_enclosing_handle(), stepRange_);

    if (!stepRangeAcc)
        return GRIB_NOT_FOUND;

    return stepRangeAcc->unpack_long(val, len);
}

int MarsStep::value_count(long* count)
{
    *count = 1;
    return 0;
}

size_t MarsStep::string_length()
{
    return 16;
}

long MarsStep::get_native_type()
{
    return GRIB_TYPE_LONG;
}

}  // namespace eccodes::accessor
