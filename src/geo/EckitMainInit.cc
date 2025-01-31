/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/runtime/Main.h"


namespace eccodes::geo
{


void eckit_main_init()
{
    struct EckitMainInit
    {
        EckitMainInit()
        {
            if (!eckit::Main::ready()) {
                static char* argv[]{ const_cast<char*>("eccodes::geo::eckit_main_init") };
                eckit::Main::initialise(1, argv);
            }
        }
    } static const eckit_main_init;
}


}  // namespace eccodes::geo
