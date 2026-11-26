// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// The version of spaec (major.minor.patch). Before 1.0.0 the interface may still change.
// CMakeLists.txt reads the version from this file: change it only here.
#pragma once

#define SPAEC_VERSION_MAJOR 0
#define SPAEC_VERSION_MINOR 0
#define SPAEC_VERSION_PATCH 1
#define SPAEC_VERSION       "0.0.1"

namespace spaec
{

// The version of the library, e.g. "0.0.1"
constexpr const char* version()
{
    return SPAEC_VERSION;
}

}  // namespace spaec
