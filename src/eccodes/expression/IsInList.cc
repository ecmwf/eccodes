/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "IsInList.h"
#include "eccodes_prototypes.h"

namespace eccodes::expression {

Dict IsInList::load_list(grib_context* c,  int* err) const
{
    char* filename  = NULL;
    char line[1024] = {0,};
    Dict list;
    FILE* f         = NULL;

    *err = GRIB_SUCCESS;

    filename = grib_context_full_defs_path(c, list_);
    if (!filename) {
        grib_context_log(c, GRIB_LOG_ERROR, "unable to find def file %s", list_);
        *err = GRIB_FILE_NOT_FOUND;
        return list;
    }
    else {
        grib_context_log(c, GRIB_LOG_DEBUG, "is_in_list: found def file %s", filename);
    }

    if (c->lists.find(filename) != c->lists.end()) {
        list = c->lists[filename];
        grib_context_log(c, GRIB_LOG_DEBUG, "using list %s from cache", filename);
        return list;
    }
    else {
        grib_context_log(c, GRIB_LOG_DEBUG, "using list %s from file %s", list_, filename);
    }

    f = codes_fopen(filename, "r");
    if (!f) {
        *err = GRIB_IO_PROBLEM;
        return list;
    }

    while (fgets(line, sizeof(line) - 1, f)) {
        unsigned char* p = (unsigned char*)line;
        while (*p != 0) {
            if (*p < 33) {
                *p = 0;
                break;
            }
            p++;
        }
        if (list.find(line) == list.end()) {
          List l = list[line];
          for (auto& item : l) {
            if (item) {
                grib_context_free(c, item);
            }
          }
          l.clear();
        }
        char* copy_line = (char*)grib_context_malloc_clear(c, strlen(line) + 1);
        memcpy(copy_line, line, strlen(line));
        list[copy_line] = { strdup(copy_line) };
    }
    c->lists[filename] = list;

    fclose(f);

    return list;
}

const char* IsInList::get_name() const
{
    return name_;
}

int IsInList::evaluate_long(grib_handle* h, long* result) const
{
    int err = 0;
    char mybuf[1024] = {0,};
    size_t size = 1024;

    Dict list = load_list(h->context, &err);

    if ((err = grib_get_string_internal(h, name_, mybuf, &size)) != GRIB_SUCCESS)
        return err;

    if (list.find(mybuf) != list.end())
        *result = 1;
    else
        *result = 0;

    return err;
}

int IsInList::evaluate_double(grib_handle* h, double* result) const
{
    return GRIB_NOT_IMPLEMENTED;
    // grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
    // int err                       = 0;
    // char mybuf[1024]              = {0,};
    // size_t size = 1024;
    // grib_trie* list = load_list(h->context, g, &err);
    // if ((err = grib_get_string_internal(h, name_, mybuf, &size)) != GRIB_SUCCESS)
    //     return err;
    // if (grib_trie_get(list, mybuf))
    //     *result = 1;
    // else
    //     *result = 0;
    // return err;
}

const char* IsInList::evaluate_string(grib_handle* h, char* buf, size_t* size, int* err) const
{
    char mybuf[1024] = {0,};
    size_t sizebuf = 1024;
    long result;

    Dict list = load_list(h->context, err);

    if ((*err = grib_get_string_internal(h, name_, mybuf, &sizebuf)) != GRIB_SUCCESS)
        return NULL;

    if (list.find(mybuf) != list.end())
        result = 1;
    else
        result = 0;

    snprintf(buf, 32, "%ld", result);
    *size = strlen(buf);
    return buf;
}

void IsInList::print(grib_context* c, grib_handle* f, FILE* out) const
{
    fprintf(out, "access('%s", name_);
    if (f) {
        long s = 0;
        grib_get_long(f, name_, &s);
        fprintf(out, "=%ld", s);
    }
    fprintf(out, "')");
}


void IsInList::add_dependency(grib_accessor* observer)
{
    grib_accessor* observed = grib_find_accessor(observer->get_enclosing_handle(), name_);

    if (!observed) {
        /* grib_context_log(observer->context, GRIB_LOG_ERROR, */
        /* "Error in accessor_add_dependency: cannot find [%s]", name_); */
        /* ECCODES_ASSERT(observed); */
        return;
    }

    grib_dependency_add(observer, observed);
}

void IsInList::destroy(grib_context* c) {
    grib_context_free_persistent(c, name_);
    grib_context_free_persistent(c, list_);
}

IsInList::IsInList(grib_context* c, const char* name, const char* list)
{
    name_ = grib_context_strdup_persistent(c, name);
    list_ = grib_context_strdup_persistent(c, list);
}

int IsInList::native_type(grib_handle* h) const
{
    int type = 0;
    int err;
    if ((err = grib_get_native_type(h, name_, &type)) != GRIB_SUCCESS)
        grib_context_log(h->context, GRIB_LOG_ERROR,
                         "Error in native_type %s : %s", name_, grib_get_error_message(err));
    return type;
}

}  // namespace eccodes::expression

grib_expression* new_is_in_list_expression(grib_context* c, const char* name, const char* list) {
    return new eccodes::expression::IsInList(c, name, list);
}
