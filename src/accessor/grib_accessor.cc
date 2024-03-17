/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005
 ***************************************************************************/

#include "grib_accessor.h"
#include <iostream>

// Note: A fast cut-down version of strcmp which does NOT return -1
// 0 means input strings are equal and 1 means not equal
GRIB_INLINE static int grib_inline_strcmp(const char* a, const char* b)
{
    if (*a != *b)
        return 1;
    while ((*a != 0 && *b != 0) && *(a) == *(b)) {
        a++;
        b++;
    }
    return (*a == 0 && *b == 0) ? 0 : 1;
}

template <>
int grib_accessor::unpack<double>(double* v, size_t* len)
{
    return unpack_double(v, len);
}

template <>
int grib_accessor::unpack<float>(float* v, size_t* len)
{
    return unpack_float(v, len);
}

int grib_accessor::compare_accessors(grib_accessor* a2, int compare_flags)
{
    int ret                 = 0;
    long type1              = 0;
    long type2              = 0;
    int type_mismatch       = 0;
    grib_accessor_class* c1 = NULL;
    grib_accessor* a1       = this;

    if ((compare_flags & GRIB_COMPARE_NAMES) && grib_inline_strcmp(a1->name, a2->name))
        return GRIB_NAME_MISMATCH;

    if (compare_flags & GRIB_COMPARE_TYPES) {
        type1 = a1->get_native_type();
        type2 = a2->get_native_type();

        type_mismatch = type1 != type2 ? 1 : 0;
    }

    // ret = GRIB_UNABLE_TO_COMPARE_ACCESSORS;
    c1  = a1->cclass;
    ret = c1->compare(a1, a2);

    if (ret == GRIB_VALUE_MISMATCH && type_mismatch)
        ret = GRIB_TYPE_AND_VALUE_MISMATCH;

    return ret;
}

int grib_accessor::add_attribute(grib_accessor* attr, int nest_if_clash)
{
    int id              = 0;
    int idx             = 0;
    grib_accessor* same = NULL;
    grib_accessor* aloc = this;

    if (this->has_attributes()) {
        same = this->get_attribute_index(attr->name, &id);
    }

    if (same) {
        if (nest_if_clash == 0)
            return GRIB_ATTRIBUTE_CLASH;
        aloc = same;
    }

    for (id = 0; id < MAX_ACCESSOR_ATTRIBUTES; id++) {
        if (aloc->attributes[id] == NULL) {
            // attr->parent=a->parent;
            aloc->attributes[id]      = attr;
            attr->parent_as_attribute = aloc;
            if (aloc->same)
                attr->same = aloc->same->get_attribute_index(attr->name, &idx);

            grib_context_log(this->context, GRIB_LOG_DEBUG, "added attribute %s->%s", this->name, attr->name);
            return GRIB_SUCCESS;
        }
    }
    return GRIB_TOO_MANY_ATTRIBUTES;
}

grib_accessor* grib_accessor::get_attribute_index(const char* name, int* index)
{
    int i = 0;
    while (i < MAX_ACCESSOR_ATTRIBUTES && this->attributes[i]) {
        if (!grib_inline_strcmp(this->attributes[i]->name, name)) {
            *index = i;
            return this->attributes[i];
        }
        i++;
    }
    return NULL;
}

int grib_accessor::has_attributes()
{
    return this->attributes[0] ? 1 : 0;
}

grib_accessor* grib_accessor::get_attribute(const char* name)
{
    int index                  = 0;
    const char* p              = 0;
    char* basename             = NULL;
    const char* attribute_name = NULL;
    grib_accessor* acc         = NULL;
    p                          = name;
    while (*(p + 1) != '\0' && (*p != '-' || *(p + 1) != '>'))
        p++;
    if (*(p + 1) == '\0') {
        return this->get_attribute_index(name, &index);
    }
    else {
        size_t size    = p - name;
        attribute_name = p + 2;
        basename       = (char*)grib_context_malloc_clear(this->context, size + 1);
        basename       = (char*)memcpy(basename, name, size);
        acc            = this->get_attribute_index(basename, &index);
        grib_context_free(this->context, basename);
        if (acc)
            return acc->get_attribute(attribute_name);
        else
            return NULL;
    }
}
