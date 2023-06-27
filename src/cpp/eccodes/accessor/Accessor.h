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


class Accessor /* eckit non copyable : public Accessor */
{
protected:
    Accessor(const long len, grib_arguments* param);
    virtual ~Accessor();

public:

    // Legacy methods

    virtual void dump(grib_dumper* dumper) const =0;
    virtual long next_offset() const =0;
    virtual int value_count(long* count) const =0;
    virtual size_t string_length() const =0;
    virtual long byte_count() const =0;
    virtual int get_native_type() const =0;
    virtual long byte_offset() const =0;
    virtual int unpack_bytes(unsigned char* val, size_t* len) const =0;
    virtual int clear() const =0;
    virtual int unpack_long(long* v, size_t* len) const =0;
    virtual int unpack_double(double* v, size_t* len) const =0;
    virtual int unpack_float(float* v, size_t* len) const =0;
    virtual int unpack_string(char* v, size_t* len) const =0;
    virtual int unpack_string_array(char** v, size_t* len) const =0;
    virtual int pack_expression(grib_expression* e) const =0;
    virtual int pack_long(const long* v, size_t* len) const =0;
    virtual int pack_double(const double* v, size_t* len) const =0;
    virtual int pack_string_array(const char** v, size_t* len) const =0;
    virtual int pack_string(const char* v, size_t* len) const =0;
    virtual int pack_bytes(const unsigned char* val, size_t* len) const =0;
    virtual grib_section* sub_section() const =0;
    virtual int notify_change(grib_accessor* observed) const =0;
    virtual void update_size(size_t s) const =0;
    virtual grib_accessor* next(int mod) const =0;
    virtual int compare(const Accessor*) const =0;
    virtual size_t preferred_size(int from_handle) const =0;
    virtual int is_missing() const =0;
    virtual int unpack_double_element(size_t i, double* val) const =0;
    virtual int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) const =0;
    virtual int unpack_double_subarray(double* val, size_t start, size_t len) const =0;
    virtual grib_accessor* make_clone(grib_section* s, int* err) const =0;
    virtual void print(std::ostream& s) const =0;

    // Missing from the original

    virtual int pack_missing() const;


    // Members


    // friends
    friend std::ostream& operator<<(std::ostream& s, const Accessor& p)
    {
        p.print(s);
        return s;
    }


    // For now....
    public:

    grib_context *context_;
    unsigned long flags_;
    const char* name;

    size_t offset_;

    // mutable == bug
    mutable size_t length_;

    grib_handle* handle() const;
    grib_handle* h;
    Accessor* parent;
    int carg;



};



class AccessorFactory {
    std::string name_;
    virtual Accessor* make(long length, grib_arguments* args) = 0;

    AccessorFactory(const AccessorFactory&)            = delete;
    AccessorFactory& operator=(const AccessorFactory&) = delete;

protected:
    AccessorFactory(const std::string&);
    virtual ~AccessorFactory();

public:
    static const Accessor* build(long length, grib_arguments* args);
    static void list(std::ostream&);
};


template <class T>
class AccessorMaker : public AccessorFactory {
    Accessor* make(long length, grib_arguments* args) override { return new T(length, args); }

public:
    AccessorMaker(const std::string& name) : AccessorFactory(name) {}
};


}  // namespace accessor
}  // namespace eccodes
