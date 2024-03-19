
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
#include "grib_accessor_class_smart_table_column.h"
#include "grib_accessor_class_smart_table.h"

grib_accessor_class_smart_table_column_t _grib_accessor_class_smart_table_column{"smart_table_column"};
grib_accessor_class* grib_accessor_class_smart_table_column = &_grib_accessor_class_smart_table_column;



void grib_accessor_class_smart_table_column_t::init(grib_accessor* a, const long len, grib_arguments* params){
    grib_accessor_class_gen_t::init(a, len, params);
    int n = 0;
    grib_accessor_smart_table_column_t* self = (grib_accessor_smart_table_column_t*)a;

    self->smartTable = grib_arguments_get_name(grib_handle_of_accessor(a), params, n++);
    self->index = grib_arguments_get_long(grib_handle_of_accessor(a), params, n++);

    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void grib_accessor_class_smart_table_column_t::dump(grib_accessor* a, grib_dumper* dumper){
    int type = get_native_type(a);

    switch (type) {
        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, a, NULL);
            break;
        case GRIB_TYPE_STRING:
            grib_dump_string_array(dumper, a, NULL);
            break;
    }
}

int grib_accessor_class_smart_table_column_t::unpack_string_array(grib_accessor* a, char** buffer, size_t* len){
    grib_accessor_smart_table_column_t* self   = (grib_accessor_smart_table_column_t*)a;
    grib_accessor_smart_table_t* tableAccessor = NULL;
    grib_smart_table* table = NULL;

    size_t size = 1;
    long* code;
    int err = GRIB_SUCCESS;
    char tmp[1024] = {0,};
    int i = 0;

    tableAccessor = (grib_accessor_smart_table_t*)grib_find_accessor(grib_handle_of_accessor(a), self->smartTable);
    if (!tableAccessor) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Unable to find accessor %s", self->smartTable);
        return GRIB_NOT_FOUND;
    }

    err = grib_get_size_acc(grib_handle_of_accessor(a), (grib_accessor*)tableAccessor, &size);
    if (err)
        return err;
    if (*len < size) {
        return GRIB_BUFFER_TOO_SMALL;
    }

    code = (long*)grib_context_malloc_clear(a->context, sizeof(long) * size);
    if (!code) {
        grib_context_log(a->context, GRIB_LOG_FATAL, "%s: Memory allocation error: %zu bytes", a->name, size);
        return GRIB_OUT_OF_MEMORY;
    }

    if ((err = ((grib_accessor*)tableAccessor)->unpack_long(code, &size)) != GRIB_SUCCESS)
        return err;

    table = tableAccessor->table;

    for (i = 0; i < size; i++) {
        if (table && (code[i] >= 0) &&
            (code[i] < table->numberOfEntries) &&
            table->entries[code[i]].column[self->index]) {
            strcpy(tmp, table->entries[code[i]].column[self->index]);
        }
        else {
            snprintf(tmp, sizeof(tmp), "%d", (int)code[i]);
        }

        buffer[i] = grib_context_strdup(a->context, tmp);
    }
    *len = size;
    grib_context_free(a->context, code);

    return GRIB_SUCCESS;
}

int grib_accessor_class_smart_table_column_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_smart_table_column_t* self   = (grib_accessor_smart_table_column_t*)a;
    grib_accessor_smart_table_t* tableAccessor = NULL;
    grib_smart_table* table = NULL;

    size_t size = 1;
    long* code;
    int err = GRIB_SUCCESS;
    int i   = 0;

    for (i = 0; i < *len; i++)
        val[i] = GRIB_MISSING_LONG;

    tableAccessor = (grib_accessor_smart_table_t*)grib_find_accessor(grib_handle_of_accessor(a), self->smartTable);
    if (!tableAccessor) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Unable to find accessor %s", self->smartTable);
        return GRIB_NOT_FOUND;
    }

    err = grib_get_size_acc(grib_handle_of_accessor(a), (grib_accessor*)tableAccessor, &size);
    if (err)
        return err;
    if (*len < size) {
        return GRIB_BUFFER_TOO_SMALL;
    }

    code = (long*)grib_context_malloc_clear(a->context, sizeof(long) * size);
    if (!code) return GRIB_OUT_OF_MEMORY;

    if ((err = ((grib_accessor*)tableAccessor)->unpack_long(code, &size)) != GRIB_SUCCESS) {
        grib_context_free(a->context, code);
        return err;
    }

    table = tableAccessor->table;

    for (i = 0; i < size; i++) {
        if (table && (code[i] >= 0) &&
            (code[i] < table->numberOfEntries) &&
            table->entries[code[i]].column[self->index]) {
            val[i] = atol(table->entries[code[i]].column[self->index]);
        }
    }
    *len = size;
    grib_context_free(a->context, code);

    return GRIB_SUCCESS;
}

int grib_accessor_class_smart_table_column_t::value_count(grib_accessor* a, long* count){
    grib_accessor_smart_table_column_t* self = (grib_accessor_smart_table_column_t*)a;
    size_t size = 0;
    int err = 0;
    *count = 0;

    if (!self->smartTable)
        return 0;

    err = grib_get_size(grib_handle_of_accessor(a), self->smartTable, &size);
    *count = size;
    return err;
}

void grib_accessor_class_smart_table_column_t::destroy(grib_context* context, grib_accessor* a){
    if (a->vvalue != NULL) {
        grib_context_free(context, a->vvalue);
        a->vvalue = NULL;
    }
}

int grib_accessor_class_smart_table_column_t::get_native_type(grib_accessor* a){
    int type = GRIB_TYPE_LONG;
    /*printf("---------- %s flags=%ld GRIB_ACCESSOR_FLAG_STRING_TYPE=%d\n",
         a->name,a->flags,GRIB_ACCESSOR_FLAG_STRING_TYPE);*/
    if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type = GRIB_TYPE_STRING;
    return type;
}
