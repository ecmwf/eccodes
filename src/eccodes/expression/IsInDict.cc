/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "IsInDict.h"

namespace eccodes::expression {

std::shared_ptr<Dict> IsInDict::load_dictionary(grib_context* c, int* err) const
{
    char* filename  = NULL;
    char line[1024] = {0,};
    char key[1024] = {0,};
    FILE* f = NULL;
    int i = 0;
    std::shared_ptr<Dict> dictionary;

    *err = GRIB_SUCCESS;

    filename = grib_context_full_defs_path(c, dictionary_);
    if (!filename) {
        grib_context_log(c, GRIB_LOG_ERROR, "unable to find def file %s", dictionary_);
        *err = GRIB_FILE_NOT_FOUND;
        return nullptr;
    }
    else {
        grib_context_log(c, GRIB_LOG_DEBUG, "is_in_dict: found def file %s", filename);
    }

    if (c->lists.find(filename) != c->lists.end()) {
        dictionary = c->lists[filename];
        grib_context_log(c, GRIB_LOG_DEBUG, "using dictionary %s from cache", dictionary_);
        return dictionary;
    }
    else {
        grib_context_log(c, GRIB_LOG_DEBUG, "using dictionary %s from file %s", dictionary_, filename);
    }

    f = codes_fopen(filename, "r");
    if (!f) {
        *err = GRIB_IO_PROBLEM;
        return nullptr;
    }

    dictionary = std::make_shared<Dict>();

    while (fgets(line, sizeof(line) - 1, f)) {
        i = 0;
        while (line[i] != '|' && line[i] != 0) {
            key[i] = line[i];
            i++;
        }
        key[i] = 0;
        (*dictionary)[key] = List{line};
    }

    c->lists[filename] = dictionary;

    fclose(f);

    return dictionary;
}

const char* IsInDict::get_name() const
{
    return key_;
}

int IsInDict::evaluate_long(grib_handle* h, long* result) const
{
    int err = 0;
    char mybuf[1024] = {0,};
    size_t size = 1024;

    const auto dict = load_dictionary(h->context, &err);

    if ((err = grib_get_string_internal(h, key_, mybuf, &size)) != GRIB_SUCCESS)
        return err;

    if (dict != nullptr)
    {
        if (dict->find(mybuf) != dict->end())
            *result = 1;
        else
            *result = 0;
    }
    else
    {
        *result = 0;
    }

    return err;
}

int IsInDict::evaluate_double(grib_handle* h, double* result) const
{
    return GRIB_NOT_IMPLEMENTED;

    // grib_expression_is_in_dict* e = (grib_expression_is_in_dict*)g;
    // int err                       = 0;
    // char mybuf[1024]              = {0,};
    // size_t size = 1024;
    // grib_trie* list = load_dictionary(h->context, g, &err);
    // if ((err = grib_get_string_internal(h, key_, mybuf, &size)) != GRIB_SUCCESS)
    //     return err;
    // if (grib_trie_get(list, mybuf))
    //     *result = 1;
    // else
    //     *result = 0;
    // return err;
}

const char* IsInDict::evaluate_string(grib_handle* h, char* buf, size_t* size, int* err) const
{
    *err = GRIB_NOT_IMPLEMENTED;
    return NULL;

    // grib_expression_is_in_dict* e = (grib_expression_is_in_dict*)g;
    // char mybuf[1024]              = {0,};
    // size_t sizebuf = 1024;
    // long result;
    // grib_trie* list = load_dictionary(h->context, g, err);
    // if ((*err = grib_get_string_internal(h, key_, mybuf, &sizebuf)) != GRIB_SUCCESS)
    //     return NULL;
    // if (grib_trie_get(list, mybuf))
    //     result = 1;
    // else
    //     result = 0;
    // snprintf(buf, 32, "%ld", result);
    // *size = strlen(buf);
    // return buf;
}

void IsInDict::print(grib_context* c, grib_handle* f, FILE* out) const
{
    fprintf(out, "access('%s", key_);
    if (f) {
        long s = 0;
        grib_get_long(f, key_, &s);
        fprintf(out, "=%ld", s);
    }
    fprintf(out, "')");
}

IsInDict::IsInDict(grib_context* c, const char* name, const char* list)
{
    key_        = grib_context_strdup_persistent(c, name);
    dictionary_ = grib_context_strdup_persistent(c, list);
}

int IsInDict::native_type(grib_handle* h) const
{
    return GRIB_TYPE_LONG;
}

void IsInDict::add_dependency(grib_accessor* observer)
{
    grib_accessor* observed = grib_find_accessor(observer->get_enclosing_handle(), key_);

    if (!observed) {
        /* grib_context_log(observer->context, GRIB_LOG_ERROR, */
        /* "Error in accessor_add_dependency: cannot find [%s]", name_); */
        /* ECCODES_ASSERT(observed); */
        return;
    }

    grib_dependency_add(observer, observed);
}

}  // namespace eccodes::expression

grib_expression* new_is_in_dict_expression(grib_context* c, const char* name, const char* list) {
    return new eccodes::expression::IsInDict(c, name, list);
}
