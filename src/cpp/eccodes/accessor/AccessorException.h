/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <exception>

#define ASSERT(a) /* */
#define DEBUG_ASSERT(a) /* */

namespace eccodes::accessor
{

class AccessorException : public std::exception{
    int code_;
public:
    AccessorException(int code) : code_(code) {}
    int code() const { return code_; }
};

}  // namespace eccodes::accessor
