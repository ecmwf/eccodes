/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "SectionPointer.h"

eccodes::AccessorBuilder<eccodes::accessor::SectionPointer> _grib_accessor_section_pointer_builder{};

namespace eccodes::accessor
{

void SectionPointer::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    grib_handle* hand = get_enclosing_handle();

    int n = 0;
    sectionOffset_ = arg->get_name(hand, n++);
    sectionLength_ = arg->get_name(hand, n++);
    sectionNumber_ = arg->get_long(hand, n++);

    ECCODES_ASSERT(sectionNumber_ < MAX_NUM_SECTIONS);

    hand->section_offset[sectionNumber_] = (char*)sectionOffset_;
    hand->section_length[sectionNumber_] = (char*)sectionLength_;

    /* printf("++++++++++++++ GRIB_API:  creating section_pointer%d %s %s\n", */
    /* sectionNumber,sectionLength,sectionLength_ ); */

    if (hand->sections_count < sectionNumber_)
        hand->sections_count = sectionNumber_;

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_HIDDEN;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    length_ = 0;
}

long SectionPointer::get_native_type()
{
    return GRIB_TYPE_BYTES;
}

int SectionPointer::unpack_string(char* v, size_t* len)
{
    //   unsigned char* p=NULL;
    //   char* s=v;
    //   int i;
    //   long length=byte_count();
    //   if (*len < length) return GRIB_ARRAY_TOO_SMALL;
    //
    //   p = get_enclosing_handle()->buffer->data + byte_offset();
    //   for (i = 0; i < length; i++)  {
    //     snprintf (s,64,"%02x", *(p++));
    //     s+=2;
    //   }
    //   *len=length;

    snprintf(v, 64, "%ld_%ld", byte_offset(), byte_count());
    return GRIB_SUCCESS;
}

long SectionPointer::byte_count()
{
    long sectionLength = 0;

    int ret = grib_get_long(get_enclosing_handle(), sectionLength_, &sectionLength);
    if (ret) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "Unable to get %s %s",
                         sectionLength_, grib_get_error_message(ret));
        return -1;
    }

    return sectionLength;
}

long SectionPointer::byte_offset()
{
    long sectionOffset = 0;

    int ret = grib_get_long(get_enclosing_handle(), sectionOffset_, &sectionOffset);
    if (ret) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "Unable to get %s %s",
                         sectionOffset_, grib_get_error_message(ret));
        return -1;
    }

    return sectionOffset;
}

}  // namespace eccodes::accessor
