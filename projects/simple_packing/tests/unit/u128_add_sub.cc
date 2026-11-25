// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "simple_packing/detail/U128.h"

#include <cassert>

using namespace sp::detail;


void test_subtraction()
{
    {
        [[maybe_unused]] UInt128 a{ 0, 0 };
        [[maybe_unused]] UInt128 ref{ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
        assert(a - 1 == ref);
    }

    {
        [[maybe_unused]] UInt128 a{ 1, 0 };
        [[maybe_unused]] UInt128 ref{ 0, 0xFFFFFFFFFFFFFFFF };
        assert(a - 1 == ref);
    }
    {
        [[maybe_unused]] UInt128 a{ 0, 0xFFFFFFFFFFFFFFFF };
        [[maybe_unused]] UInt128 ref{ 0, 0xFFFFFFFFFFFFFFFE };
        assert(a - 1 == ref);
    }
}

void test_addition()
{
    {
        [[maybe_unused]] UInt128 a{ 0, 0 };
        [[maybe_unused]] UInt128 ref{ 0, 1 };
        assert(a + 1 == ref);
    }
    {
        [[maybe_unused]] UInt128 a{ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
        [[maybe_unused]] UInt128 ref{ 0, 0 };
        assert(a + 1 == ref);
    }
    {
        [[maybe_unused]] UInt128 a{ 0, 0xFFFFFFFFFFFFFFFF };
        [[maybe_unused]] UInt128 ref{ 1, 0 };
        assert(a + 1 == ref);
    }
}

int main(int /*argc*/, char* /*argv*/[])
{
    test_subtraction();
    test_addition();
    return 0;
}
