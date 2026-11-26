// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// The floating-point compiler options of the tests (README.md, "Floating-point compiler options"):
// no fused multiply-adds, no -ffast-math, not the default 'fast' model of icx/icpx. Fails with an
// explanation and the CMake option to set if one of the options this compiler needs is missing.

#include <iostream>

#include "../fp_diagnostics.h"

int main()
{
    if (fp_diagnostics::explain()) {
        return 1;
    }
    std::cout << "floating-point options ok: " SPAEC_TEST_FP_FLAGS "\n";
    return 0;
}
