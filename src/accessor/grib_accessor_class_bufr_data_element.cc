
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
#include "grib_accessor_class_bufr_data_element.h"

grib_accessor_class_bufr_data_element_t _grib_accessor_class_bufr_data_element{"bufr_data_element"};
grib_accessor_class* grib_accessor_class_bufr_data_element = &_grib_accessor_class_bufr_data_element;


static grib_accessor* make_clone(grib_accessor* a, grib_section* s, int* err)
{
    grib_accessor* the_clone = NULL;
    grib_accessor* attribute = NULL;
    grib_accessor_bufr_data_element_t* elementAccessor;
    grib_accessor_bufr_data_element_t* self;
    char* copied_name = NULL;
    int i;
    grib_action creator = {0,};
    creator.op         = (char*)"bufr_data_element";
    creator.name_space = (char*)"";
    creator.set        = 0;
    creator.name       = (char*)"unknown";
    if (strcmp(a->cclass->name, "bufr_data_element")) {
        grib_context_log(a->context, GRIB_LOG_FATAL, "wrong accessor type: '%s' should be '%s'", a->cclass->name, "bufr_data_element");
    }
    *err = 0;

    the_clone                                 = grib_accessor_factory(s, &creator, 0, NULL);
    copied_name                               = grib_context_strdup(a->context, a->name);
    the_clone->name                           = copied_name;
    elementAccessor                           = (grib_accessor_bufr_data_element_t*)the_clone;
    self                                      = (grib_accessor_bufr_data_element_t*)a;
    the_clone->flags                          = a->flags;
    the_clone->parent                         = NULL;
    the_clone->h                              = s->h;
    elementAccessor->index                    = self->index;
    elementAccessor->type                     = self->type;
    elementAccessor->numberOfSubsets          = self->numberOfSubsets;
    elementAccessor->subsetNumber             = self->subsetNumber;
    elementAccessor->compressedData           = self->compressedData;
    elementAccessor->descriptors              = self->descriptors;
    elementAccessor->numericValues            = self->numericValues;
    elementAccessor->stringValues             = self->stringValues;
    elementAccessor->elementsDescriptorsIndex = self->elementsDescriptorsIndex;
    elementAccessor->cname                    = copied_name; /* ECC-765 */

    i = 0;
    while (a->attributes[i]) {
        attribute = a->attributes[i]->clone(s, err);
        /* attribute->parent=a->parent; */
        the_clone->add_attribute(attribute, 0);
        i++;
    }

    return the_clone;
}

void accessor_bufr_data_element_set_index(grib_accessor* a, long index)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->index = index;
}

void accessor_bufr_data_element_set_type(grib_accessor* a, int type)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->type = type;
}

void accessor_bufr_data_element_set_numberOfSubsets(grib_accessor* a, long numberOfSubsets)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->numberOfSubsets = numberOfSubsets;
}

void accessor_bufr_data_element_set_subsetNumber(grib_accessor* a, long subsetNumber)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->subsetNumber = subsetNumber;
}

void accessor_bufr_data_element_set_compressedData(grib_accessor* a, int compressedData)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->compressedData = compressedData;
}

void accessor_bufr_data_element_set_descriptors(grib_accessor* a, bufr_descriptors_array* descriptors)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->descriptors = descriptors;
}

void accessor_bufr_data_element_set_numericValues(grib_accessor* a, grib_vdarray* numericValues)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->numericValues = numericValues;
}

void accessor_bufr_data_element_set_stringValues(grib_accessor* a, grib_vsarray* stringValues)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->stringValues = stringValues;
}

void accessor_bufr_data_element_set_elementsDescriptorsIndex(grib_accessor* a, grib_viarray* elementsDescriptorsIndex)
{
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    self->elementsDescriptorsIndex = elementsDescriptorsIndex;
}

void grib_accessor_class_bufr_data_element_t::init(grib_accessor* a, const long len, grib_arguments* params){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_BUFR_DATA;
    /* a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY; */
    self->cname = NULL;
}

void grib_accessor_class_bufr_data_element_t::dump(grib_accessor* a, grib_dumper* dumper){
    int type = get_native_type(a);

    switch (type) {
        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, a, NULL);
            break;
        case GRIB_TYPE_DOUBLE:
            grib_dump_values(dumper, a);
            break;
        case GRIB_TYPE_STRING:
            grib_dump_string_array(dumper, a, NULL);
            break;
    }
}

int grib_accessor_class_bufr_data_element_t::unpack_string_array(grib_accessor* a, char** val, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;

    int ret = 0, idx = 0;
    size_t count = 0, i = 0;
    grib_context* c = a->context;

    if (self->compressedData) {
        DEBUG_ASSERT(self->index < self->numericValues->n);
        idx = ((int)self->numericValues->v[self->index]->v[0] / 1000 - 1) / self->numberOfSubsets;
        DEBUG_ASSERT(idx < self->stringValues->n);
        count = grib_sarray_used_size(self->stringValues->v[idx]);
        for (i = 0; i < count; i++) {
            val[i] = grib_context_strdup(c, self->stringValues->v[idx]->v[i]);
        }
        *len = count;
    }
    else {
        DEBUG_ASSERT(self->subsetNumber < self->numericValues->n);
        DEBUG_ASSERT(self->index < self->numericValues->v[self->subsetNumber]->n);
        idx = (int)self->numericValues->v[self->subsetNumber]->v[self->index] / 1000 - 1;
        val[0] = grib_context_strdup(c, self->stringValues->v[idx]->v[0]);
        *len   = 1;
    }

    return ret;
}

int grib_accessor_class_bufr_data_element_t::pack_string_array(grib_accessor* a, const char** v, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;

    int ret = GRIB_SUCCESS, idx = 0;
    size_t i = 0;
    char* s  = NULL;
    grib_context* c = a->context;

    if (self->compressedData) {
        idx = ((int)self->numericValues->v[self->index]->v[0] / 1000 - 1) / self->numberOfSubsets;
        if (*len != 1 && *len != (size_t)self->numberOfSubsets) {
            grib_context_log(c, GRIB_LOG_ERROR, "Number of values mismatch for '%s': %ld strings provided but expected %ld (=number of subsets)",
                             self->descriptors->v[self->elementsDescriptorsIndex->v[0]->v[idx]]->shortName, *len, self->numberOfSubsets);
            return GRIB_ARRAY_TOO_SMALL;
        }
        grib_sarray_delete_content(c, self->stringValues->v[idx]); /* ECC-1172 */
        grib_sarray_delete(c, self->stringValues->v[idx]);
        self->stringValues->v[idx] = grib_sarray_new(c, *len, 1);
        for (i = 0; i < *len; i++) {
            s = grib_context_strdup(c, v[i]);
            grib_sarray_push(c, self->stringValues->v[idx], s);
        }
    }
    else {
        //ECC-1623
        if (*len != (size_t)self->numberOfSubsets) {
            grib_context_log(c, GRIB_LOG_ERROR,
                "Number of values mismatch for '%s': %zu strings provided but expected %ld (=number of subsets)",
                a->name, *len, self->numberOfSubsets);
            return GRIB_WRONG_ARRAY_SIZE;
        }
        for (i = 0; i < *len; i++) {
            //idx = (int)self->numericValues->v[self->subsetNumber]->v[self->index] / 1000 - 1;
            idx = (int)self->numericValues->v[i]->v[self->index] / 1000 - 1;
            self->stringValues->v[idx]->v[0] = strdup(v[i]);
        }
        *len=1;
    }

    return ret;
}

int grib_accessor_class_bufr_data_element_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    char* str = NULL;
    char* p = 0;
    size_t slen = 0;
    double dval = 0;
    size_t dlen = 1;
    int idx = 0, err = 0;
    grib_context* c = a->context;

    if (self->type != BUFR_DESCRIPTOR_TYPE_STRING) {
        char sval[32] = {0,};
        err = unpack_double(a, &dval, &dlen);
        if (err) return err;
        snprintf(sval, sizeof(sval), "%g", dval);
        slen = strlen(sval);
        if (*len < slen)
            return GRIB_BUFFER_TOO_SMALL;
        strcpy(val, sval);
        return GRIB_SUCCESS;
    }

    if (self->compressedData) {
        DEBUG_ASSERT(self->index < self->numericValues->n);
        idx = ((int)self->numericValues->v[self->index]->v[0] / 1000 - 1) / self->numberOfSubsets;
        if (idx < 0)
            return GRIB_INTERNAL_ERROR;
        str = grib_context_strdup(c, self->stringValues->v[idx]->v[0]);
    }
    else {
        DEBUG_ASSERT(self->subsetNumber < self->numericValues->n);
        idx = (int)self->numericValues->v[self->subsetNumber]->v[self->index] / 1000 - 1;
        if (idx < 0)
            return GRIB_INTERNAL_ERROR;
        DEBUG_ASSERT(idx < self->stringValues->n);
        str = grib_context_strdup(c, self->stringValues->v[idx]->v[0]);
    }

    if (str == NULL || strlen(str) == 0) {
        grib_context_free(c, str);
        *len = 0;
        *val = 0;
        return GRIB_SUCCESS;
    }

    /* Start from the end of the string and remove spaces */
    p = str;
    while (*p != 0)
        p++;
    p--;
    while (p != str) {
        if (*p != ' ')
            break;
        else
            *p = 0;
        p--;
    }
    slen = strlen(str);
    if (slen > *len)
        return GRIB_ARRAY_TOO_SMALL;

    strcpy(val, str);
    grib_context_free(c, str);
    *len = slen;

    return GRIB_SUCCESS;
}

int grib_accessor_class_bufr_data_element_t::pack_string(grib_accessor* a, const char* val, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;

    int ret = GRIB_SUCCESS, idx = 0;
    char* s = NULL;
    grib_context* c = a->context;

    if (self->compressedData) {
        idx = ((int)self->numericValues->v[self->index]->v[0] / 1000 - 1) / self->numberOfSubsets;
    }
    else {
        idx = (int)self->numericValues->v[self->subsetNumber]->v[self->index] / 1000 - 1;
    }
    grib_sarray_delete_content(c, self->stringValues->v[idx]); /* ECC-1172 */
    grib_sarray_delete(c, self->stringValues->v[idx]);
    self->stringValues->v[idx] = grib_sarray_new(c, 1, 1);
    s = grib_context_strdup(c, val);
    grib_sarray_push(c, self->stringValues->v[idx], s);

    return ret;
}

int grib_accessor_class_bufr_data_element_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    int ret = GRIB_SUCCESS;
    long count = 0, i = 0;

    value_count(a, &count);

    if (*len < (size_t)count)
        return GRIB_ARRAY_TOO_SMALL;

    if (self->compressedData) {
        for (i = 0; i < count; i++) {
            DEBUG_ASSERT(self->index < self->numericValues->n);
            DEBUG_ASSERT(i < self->numericValues->v[self->index]->n);
            val[i] = self->numericValues->v[self->index]->v[i] == GRIB_MISSING_DOUBLE ? GRIB_MISSING_LONG : (long)self->numericValues->v[self->index]->v[i];
        }
        *len = count;
    }
    else {
        DEBUG_ASSERT(self->subsetNumber < self->numericValues->n);
        DEBUG_ASSERT(self->index < self->numericValues->v[self->subsetNumber]->n);
        val[0] = self->numericValues->v[self->subsetNumber]->v[self->index] == GRIB_MISSING_DOUBLE ? GRIB_MISSING_LONG : (long)self->numericValues->v[self->subsetNumber]->v[self->index];
        *len   = 1;
    }

    return ret;
}

int grib_accessor_class_bufr_data_element_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    int ret = GRIB_SUCCESS;
    long count = 0, i = 0;

    value_count(a, &count);

    if (*len < (size_t)count)
        return GRIB_ARRAY_TOO_SMALL;

    if (self->compressedData) {
        for (i = 0; i < count; i++) {
            DEBUG_ASSERT(self->index < self->numericValues->n);
            DEBUG_ASSERT(i < self->numericValues->v[self->index]->n);
            val[i] = self->numericValues->v[self->index]->v[i];
        }
        *len = count;
    }
    else {
        DEBUG_ASSERT(self->subsetNumber < self->numericValues->n);
        DEBUG_ASSERT(self->index < self->numericValues->v[self->subsetNumber]->n);
        val[0] = self->numericValues->v[self->subsetNumber]->v[self->index];
        *len   = 1;
    }

    return ret;
}

int grib_accessor_class_bufr_data_element_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    int ret = GRIB_SUCCESS;
    size_t count = 1, i = 0;
    grib_context* c = a->context;

    if (self->compressedData) {
        count = *len;
        if (count != 1 && count != (size_t)self->numberOfSubsets) {
            grib_context_log(c, GRIB_LOG_ERROR, "Number of values mismatch for '%s': %ld doubles provided but expected %ld (=number of subsets)",
                             self->descriptors->v[self->elementsDescriptorsIndex->v[0]->v[self->index]]->shortName, count, self->numberOfSubsets);
            return GRIB_ARRAY_TOO_SMALL;
        }
        grib_darray_delete(a->context, self->numericValues->v[self->index]);
        self->numericValues->v[self->index] = grib_darray_new(a->context, count, 1);

        for (i = 0; i < count; i++)
            grib_darray_push(a->context, self->numericValues->v[self->index], val[i]);

        *len = count;
    }
    else {
        self->numericValues->v[self->subsetNumber]->v[self->index] = val[0];
        *len = 1;
    }

    return ret;
}

int grib_accessor_class_bufr_data_element_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    int ret = 0;
    size_t count = 1, i = 0;
    grib_context* c = a->context;

    if (self->compressedData) {
        count = *len;
        if (count != 1 && count != (size_t)self->numberOfSubsets) {
            grib_context_log(c, GRIB_LOG_ERROR, "Number of values mismatch for '%s': %zu integers provided but expected %ld (=number of subsets)",
                             self->descriptors->v[self->elementsDescriptorsIndex->v[0]->v[self->index]]->shortName, count, self->numberOfSubsets);
            return GRIB_ARRAY_TOO_SMALL;
        }
        grib_darray_delete(a->context, self->numericValues->v[self->index]);
        self->numericValues->v[self->index] = grib_darray_new(a->context, count, 1);

        for (i = 0; i < count; i++) {
            grib_darray_push(a->context, self->numericValues->v[self->index], val[i] == GRIB_MISSING_LONG ? GRIB_MISSING_DOUBLE : val[i]);
        }
        *len = count;
    }
    else {
        self->numericValues->v[self->subsetNumber]->v[self->index] = val[0] == GRIB_MISSING_LONG ? GRIB_MISSING_DOUBLE : val[0];
        *len = 1;
    }

    return ret;
}

int grib_accessor_class_bufr_data_element_t::value_count(grib_accessor* a, long* count){
    int ret = 0, type = 0, idx = 0;
    size_t size = 0;
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;

    if (!self->compressedData) {
        *count = 1;
        return 0;
    }
    type = get_native_type(a);

    if (type == GRIB_TYPE_STRING) {
        DEBUG_ASSERT(self->index < self->numericValues->n);
        idx  = ((int)self->numericValues->v[self->index]->v[0] / 1000 - 1) / self->numberOfSubsets;
        size = grib_sarray_used_size(self->stringValues->v[idx]);
    }
    else {
        DEBUG_ASSERT(self->index < self->numericValues->n);
        size = grib_darray_used_size(self->numericValues->v[self->index]);
    }

    *count = size == 1 ? 1 : self->numberOfSubsets;

    return ret;
}

int grib_accessor_class_bufr_data_element_t::unpack_double_element(grib_accessor* a, size_t idx, double* val){
    /* ECC-415 */
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    int ret = GRIB_SUCCESS;
    long count = 0;

    value_count(a, &count);
    if (idx >= (size_t)count) {
        return GRIB_INTERNAL_ERROR;
    }

    if (self->compressedData) {
        *val = self->numericValues->v[self->index]->v[idx];
    }
    else {
        ret = GRIB_NOT_IMPLEMENTED;
    }
    return ret;
}

int grib_accessor_class_bufr_data_element_t::get_native_type(grib_accessor* a){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    int ret = GRIB_TYPE_DOUBLE;
    DEBUG_ASSERT(self);
    switch (self->type) {
        case BUFR_DESCRIPTOR_TYPE_STRING:
            ret = GRIB_TYPE_STRING;
            break;
        case BUFR_DESCRIPTOR_TYPE_DOUBLE:
            ret = GRIB_TYPE_DOUBLE;
            break;
        case BUFR_DESCRIPTOR_TYPE_LONG:
            ret = GRIB_TYPE_LONG;
            break;
        case BUFR_DESCRIPTOR_TYPE_TABLE:
            ret = GRIB_TYPE_LONG;
            break;
        case BUFR_DESCRIPTOR_TYPE_FLAG:
            ret = GRIB_TYPE_LONG;
            break;
    }

    return ret;
}

void grib_accessor_class_bufr_data_element_t::destroy(grib_context* ct, grib_accessor* a){
    grib_accessor_bufr_data_element_t* self = (grib_accessor_bufr_data_element_t*)a;
    int i = 0;
    if (self->cname)
        grib_context_free(ct, self->cname); /* ECC-765 */
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes[i]) {
        /*grib_context_log(ct,GRIB_LOG_DEBUG,"deleting attribute %s->%s",a->name,a->attributes[i]->name);*/
        /*printf("bufr_data_element destroy %s %p\n", a->attributes[i]->name, (void*)a->attributes[i]);*/
        a->attributes[i]->destroy(ct);
        a->attributes[i] = NULL;
        i++;
    }
}

#define MAX_STRING_SIZE 4096
/* Return 1 if BUFR element(s) is/are missing, 0 otherwise. In case of decoding errors, also return 0 */
int grib_accessor_class_bufr_data_element_t::is_missing(grib_accessor* a){
    const int ktype = get_native_type(a);
    int err = 0, result = 1; /* default: assume all are missing */
    long count = 0;
    size_t i = 0, size = 1, size2 = 0;
    grib_context* c = a->context;

    if (ktype == GRIB_TYPE_LONG) {
        long* values = NULL;
        long value = 0;

        value_count(a, &count);
        size = size2 = count;
        if (size > 1) {
            values = (long*)grib_context_malloc_clear(c, sizeof(long) * size);
            err = a->unpack_long(values, &size2);        }
        else {
            err = a->unpack_long(&value, &size2);        }
        if (err) return 0; /* TODO: no way of propagating the error up */
        Assert(size2 == size);
        if (size > 1) {
            for (i = 0; i < size; i++) {
                if (!grib_is_missing_long(a, values[i])) {
                    result = 0; /* at least one not missing */
                    break;
                }
            }
            grib_context_free(c, values);
        } else {
            result = grib_is_missing_long(a, value);
        }
    }
    else if (ktype == GRIB_TYPE_DOUBLE) {
        double value = 0;
        double* values = NULL;

        value_count(a, &count);
        size = size2 = count;
        if (size > 1) {
            values = (double*)grib_context_malloc_clear(c, sizeof(double) * size);
            err = a->unpack_double(values, &size2);        }
        else {
            err = a->unpack_double(&value, &size2);        }
        if (err) return 0; /* TODO: no way of propagating the error up */
        Assert(size2 == size);
        if (size > 1) {
            for (i = 0; i < size; ++i) {
                if (!grib_is_missing_double(a, values[i])) {
                    result = 0;
                    break;
                }
            }
            grib_context_free(c, values);
        }
        else {
            result = grib_is_missing_double(a, value);
        }
    }
    else if (ktype == GRIB_TYPE_STRING) {
        char** values = NULL;
        value_count(a, &count);
        size = count;
        if (size > 1) {
            values = (char**)grib_context_malloc_clear(a->context, size * sizeof(char*));
            err = a->unpack_string_array(values, &size);            if (err) return 0; /* TODO: no way of propagating the error up */
            for (i = 0; i < size; i++) {
                if (!grib_is_missing_string(a, (unsigned char*)values[i], size)) {
                    result = 0;
                    break;
                }
            }
            for (i = 0; i < size; i++) grib_context_free(c, values[i]);
            grib_context_free(c, values);
        } else {
            char value[MAX_STRING_SIZE] = {0,}; /* See ECC-710 */
            size = MAX_STRING_SIZE;
            err = a->unpack_string(value, &size);            if (err) return 0; /* TODO: no way of propagating the error up */
            result = grib_is_missing_string(a, (unsigned char*)value, size);
        }
    }
    else {
        return GRIB_INVALID_TYPE;
    }
    return result;
}

int grib_accessor_class_bufr_data_element_t::pack_missing(grib_accessor* a){
    int ktype = GRIB_TYPE_UNDEFINED;
    int err = 0;
    size_t size = 1;
    const int can_be_missing = (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING);
    if (!can_be_missing)
        return GRIB_VALUE_CANNOT_BE_MISSING;

    ktype = get_native_type(a);
    if (ktype == GRIB_TYPE_LONG) {
        long missing = GRIB_MISSING_LONG;
        err = pack_long(a, &missing, &size);
    }
    else if (ktype == GRIB_TYPE_DOUBLE) {
        double missing = GRIB_MISSING_DOUBLE;
        err = pack_double(a, &missing, &size);
    }
    else if (ktype == GRIB_TYPE_STRING) {
        err = pack_string(a, "", &size);
    }
    else {
        err = GRIB_INVALID_TYPE;
    }

    return err;
}
