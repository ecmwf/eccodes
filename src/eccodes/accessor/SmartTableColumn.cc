/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "SmartTableColumn.h"
#include "SmartTable.h"

eccodes::accessor::SmartTableColumn _grib_accessor_smart_table_column;
eccodes::Accessor* grib_accessor_smart_table_column = &_grib_accessor_smart_table_column;

namespace eccodes::accessor
{

void SmartTableColumn::init(const long len, grib_arguments* params)
{
    Gen::init(len, params);
    int n = 0;

    smartTable_ = params->get_name(get_enclosing_handle(), n++);
    index_      = params->get_long(get_enclosing_handle(), n++);

    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void SmartTableColumn::dump(eccodes::Dumper* dumper)
{
    int type = get_native_type();

    switch (type) {
        case GRIB_TYPE_LONG:
            dumper->dump_long(this, NULL);
            break;
        case GRIB_TYPE_STRING:
            dumper->dump_string_array(this, NULL);
            break;
    }
}

int SmartTableColumn::unpack_string_array(char** buffer, size_t* len)
{
    SmartTable* tableAccessor = NULL;
    const grib_smart_table* table = NULL;

    size_t size = 1;
    long* code;
    int err = GRIB_SUCCESS;
    char tmp[1024] = {0,};

    tableAccessor = (SmartTable*)grib_find_accessor(get_enclosing_handle(), smartTable_);
    if (!tableAccessor) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "Unable to find accessor %s", smartTable_);
        return GRIB_NOT_FOUND;
    }

    err = grib_get_size_acc(get_enclosing_handle(), (grib_accessor*)tableAccessor, &size);
    if (err)
        return err;
    if (*len < size) {
        return GRIB_BUFFER_TOO_SMALL;
    }

    code = (long*)grib_context_malloc_clear(context_, sizeof(long) * size);
    if (!code) {
        grib_context_log(context_, GRIB_LOG_FATAL, "%s: Memory allocation error: %zu bytes", name_, size);
        return GRIB_OUT_OF_MEMORY;
    }

    if ((err = ((grib_accessor*)tableAccessor)->unpack_long(code, &size)) != GRIB_SUCCESS)
        return err;

    table = tableAccessor->smarttable();

    for (size_t i = 0; i < size; i++) {
        if (table && (code[i] >= 0) &&
            (code[i] < table->numberOfEntries) &&
            table->entries[code[i]].column[index_]) {
            strcpy(tmp, table->entries[code[i]].column[index_]);
        }
        else {
            snprintf(tmp, sizeof(tmp), "%d", (int)code[i]);
        }

        buffer[i] = grib_context_strdup(context_, tmp);
    }
    *len = size;
    grib_context_free(context_, code);

    return GRIB_SUCCESS;
}

int SmartTableColumn::unpack_long(long* val, size_t* len)
{
    SmartTable* tableAccessor = NULL;
    const grib_smart_table* table = NULL;

    size_t size = 1;
    long* code;
    int err = GRIB_SUCCESS;
    size_t i = 0;

    for (i = 0; i < *len; i++)
        val[i] = GRIB_MISSING_LONG;

    tableAccessor = (SmartTable*)grib_find_accessor(get_enclosing_handle(), smartTable_);
    if (!tableAccessor) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "Unable to find accessor %s", smartTable_);
        return GRIB_NOT_FOUND;
    }

    err = grib_get_size_acc(get_enclosing_handle(), (grib_accessor*)tableAccessor, &size);
    if (err)
        return err;
    if (*len < size) {
        return GRIB_BUFFER_TOO_SMALL;
    }

    code = (long*)grib_context_malloc_clear(context_, sizeof(long) * size);
    if (!code) return GRIB_OUT_OF_MEMORY;

    if ((err = ((grib_accessor*)tableAccessor)->unpack_long(code, &size)) != GRIB_SUCCESS) {
        grib_context_free(context_, code);
        return err;
    }

    table = tableAccessor->smarttable();

    for (i = 0; i < size; i++) {
        if (table && (code[i] >= 0) &&
            (code[i] < table->numberOfEntries) &&
            table->entries[code[i]].column[index_]) {
            val[i] = atol(table->entries[code[i]].column[index_]);
        }
    }
    *len = size;
    grib_context_free(context_, code);

    return GRIB_SUCCESS;
}

int SmartTableColumn::value_count(long* count)
{
    size_t size = 0;
    int err     = 0;
    *count      = 0;

    if (!smartTable_)
        return 0;

    err = grib_get_size(get_enclosing_handle(), smartTable_, &size);
    *count = size;
    return err;
}

void SmartTableColumn::destroy(grib_context* context)
{
    if (vvalue_ != NULL) {
        grib_context_free(context, vvalue_);
        vvalue_ = NULL;
    }

    Gen::destroy(context);
}

long SmartTableColumn::get_native_type()
{
    int type = GRIB_TYPE_LONG;
    /*printf("---------- %s flags=%ld GRIB_ACCESSOR_FLAG_STRING_TYPE=%d\n",
         a->name,flags_ ,GRIB_ACCESSOR_FLAG_STRING_TYPE);*/
    if (flags_ & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type = GRIB_TYPE_STRING;
    return type;
}

}  // namespace eccodes::accessor
