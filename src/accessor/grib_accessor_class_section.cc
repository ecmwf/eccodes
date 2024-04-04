
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_section.h"

grib_accessor_class_section_t _grib_accessor_class_section{"section"};
grib_accessor_class* grib_accessor_class_section = &_grib_accessor_class_section;


void grib_accessor_class_section_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_gen_t::init(a, len, arg);
    a->sub_section = grib_section_create(grib_handle_of_accessor(a), a);
    a->length      = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void grib_accessor_class_section_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_section(dumper, a, a->sub_section->block);
}

long grib_accessor_class_section_t::byte_count(grib_accessor* a){
    if (!a->length || grib_handle_of_accessor(a)->loader) {
        if (a->name[1] == '_')
            return 0;

        /* printf("adjusting sizes SECTION %s is %ld %ld\n",a->name,(long)a->offset,(long)a->length); */
        grib_section_adjust_sizes(a->sub_section, grib_handle_of_accessor(a)->loader != NULL, 0);
        /* printf("                SECTION %s is %ld %ld\n",a->name,(long)a->offset,(long)a->length);  */
    }

    /* printf("SECTION %s is %ld %d\n",a->name,a->length,a->sub_section->aclength != NULL);  */
    return a->length;
}

long grib_accessor_class_section_t::next_offset(grib_accessor* a){
    return a->offset + a->byte_count();
}

void grib_accessor_class_section_t::destroy(grib_context* ct, grib_accessor* a){
    grib_section_delete(ct, a->sub_section);
}

int grib_accessor_class_section_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_SECTION;
}

grib_section* grib_accessor_class_section_t::sub_section(grib_accessor* a)
{
    return a->sub_section;
}

void grib_accessor_class_section_t::update_size(grib_accessor* a, size_t length){
    size_t size = 1;
    long len    = length;
    Assert(length <= 0x7fffffff);
    if (a->sub_section->aclength) {
        int e = a->sub_section->aclength->pack_long(&len, &size);        Assert(e == GRIB_SUCCESS);
        // printf("update_length %s %ld %ld\n", a->sub_section->aclength->name,
        //        (long)a->sub_section->aclength->offset,
        //        (long)a->sub_section->aclength->length);
    }

    a->sub_section->length = a->length = length;
    a->sub_section->padding            = 0;

    // printf("update_size %s %ld\n", a->name, a->length);

    Assert(a->length >= 0);
}

grib_accessor* grib_accessor_class_section_t::next(grib_accessor* a, int explore)
{
    grib_accessor* next = NULL;
    if (explore) {
        next = a->sub_section->block->first;
        if (!next)
            next = a->next_;
    }
    else {
        next = a->next_;
    }
    if (!next) {
        if (a->parent->owner)
            next = a->parent->owner->cclass->next(a->parent->owner, 0);
    }
    return next;
}
