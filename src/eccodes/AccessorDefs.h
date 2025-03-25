/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "AccessorUtils/NamedType.h"
#include <string>

namespace eccodes
{

using AccessorName = NamedType<std::string, struct NameTag>;
//using Type = NamedType<std::string, struct TypeTag>;
using AccessorNameSpace = NamedType<std::string, struct NameSpaceTag>;

} // namespace eccodes
