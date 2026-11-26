// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Translation unit of the test isa_odr compiled with -mavx2 (see isa_odr.cc)

#include <cstdint>
#include <vector>

#include "spaec/spaec.h"

using Compress = std::vector<uint8_t> (*)(const std::vector<uint16_t>&, const spaec::Options&);

Compress compressAvx2()
{
    return &spaec::compress<uint16_t>;
}
