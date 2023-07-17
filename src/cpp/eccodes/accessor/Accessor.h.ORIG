/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <cstddef>
#include <iostream>


#include "grib_api_internal.h"

#define ASSERT(a) /* */
#define DEBUG_ASSERT(a) /* */

const int TRUE  = 1;
const int FALSE = 0;
inline int grib_inline_strcmp(const char* a, const char* b) { return strcmp(a, b); }

// See https://github.com/ecmwf/mir/blob/develop/src/mir/repres/Representation.cc
// for a similar approach

namespace eccodes
{
namespace accessor
{

// Should not be here
    class EccodesException : public std::exception{
        int code_;
    public:
        EccodesException(int code) : code_(code) {}
        int code() const { return code_; }
    };


class Accessor : public grib_accessor
{
protected:
    Accessor(const long len, grib_arguments* param);
    virtual ~Accessor();

public:

    // new methods
#if 0
    virtual long unpackLong() const;
    virtual long unpackDouble() const;
    virtual std::string unpackString() const;

    // or...

    virtual void unpack(long&) const;
    virtual void unpack(double&) const;
    virtual void unpack(std::string&) const;
    virtual void unpack(std::vector<double>&) const;
#endif
    // Legacy methods

    virtual grib_accessor* make_clone(grib_section* s, int* err) const;
    virtual grib_accessor* next(int mod) const;
    virtual grib_section* sub_section() const;
    virtual int clear() const;
    virtual int compare(const Accessor*) const;
    virtual int get_native_type() const;
    virtual int is_missing() const;
    virtual int notify_change(grib_accessor* observed);

    virtual int pack_bytes(const unsigned char* val, size_t* len);
    virtual int pack_double(const double* v, size_t* len);
    virtual int pack_expression(grib_expression* e);
    virtual int pack_long(const long* v, size_t* len);
    virtual int pack_string(const char* v, size_t* len);
    virtual int pack_string_array(const char** v, size_t* len);

    virtual int unpack_bytes(unsigned char* val, size_t* len) const;
    virtual int unpack_double(double* v, size_t* len) const;
    virtual int unpack_double_element(size_t i, double* val) const;
    virtual int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) const;
    virtual int unpack_double_subarray(double* val, size_t start, size_t len) const;
    virtual int unpack_float(float* v, size_t* len) const;
    virtual int unpack_long(long* v, size_t* len) const;
    virtual int unpack_string(char* v, size_t* len) const;
    virtual int unpack_string_array(char** v, size_t* len) const;

    virtual int value_count(long* count) const;
    virtual long byte_count() const;
    virtual long byte_offset() const;
    virtual long next_offset() const;
    virtual size_t preferred_size(int from_handle) const;
    virtual size_t string_length() const;
    virtual void dump(grib_dumper* dumper) const;
    virtual void update_size(size_t s);

    virtual void print(std::ostream& s) const =0;


    virtual const char* className() const =0;

    // Missing from the original

    virtual int pack_missing();
    virtual void resize(size_t new_size) const;
    virtual int nearest_smaller_value(double val, double* nearest) const;
    virtual void post_init() const;

    // Members


    // friends
    friend std::ostream& operator<<(std::ostream& s, const Accessor& p)
    {
        p.print(s);
        return s;
    }


    // For now....
    public:

    // grib_context *context_;
    // unsigned long flags_;
    // const char* name_;

    // size_t offset_;
    // mutable size_t length_; // Updated by Bitmap

    grib_handle* handle() const { return h; }
    // grib_handle* handle() { return h; }


    // grib_section* parent_;
    // int carg;
    // grib_handle* h;
    // grib_section* parent;
    // grib_action* creator;
    // grib_virtual_value* vvalue;

    // grib_accessor* as_grib_accessor_while_converting() const;

    static Accessor* find(const grib_handle*, const char*) ;
    Accessor* find(const char*) const;

    // mutable int dirty_; // WARNING: redefine in subclasses

};

void grib_buffer_replace(const Accessor* a, const unsigned char* data, size_t newsize, int update_lengths, int update_paddings);

class AccessorFactory {
    std::string name_;
    virtual Accessor* make(long length, grib_arguments* args) = 0;

    AccessorFactory(const AccessorFactory&)            = delete;
    AccessorFactory& operator=(const AccessorFactory&) = delete;

protected:
    AccessorFactory(const std::string&);
    virtual ~AccessorFactory();

public:
    static Accessor* build(const std::string& name, long length, grib_arguments* args);
    static void list(std::ostream&);
};


template <class T>
class AccessorMaker : public AccessorFactory {
    Accessor* make(long length, grib_arguments* args) override {
         return new T(length, args); }

public:
    AccessorMaker(const std::string& name) : AccessorFactory(name) {}
};


}  // namespace accessor
}  // namespace eccodes
