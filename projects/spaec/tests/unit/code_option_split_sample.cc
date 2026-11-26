// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <iostream>

#include <exception>
#include "spaec/detail/CodeOption.h"

using namespace spaec::detail;

template <size_t K>
void testK1()
{
    // Basic
    {
        SplitSampleCodeOption co(1, K, CodeOptionSet::Basic);
        assert(co.length == 3);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption co(2, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(3, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(4, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(5, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(6, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(7, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(8, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(9, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(10, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(11, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(12, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(13, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(14, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(15, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(16, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }

    // Restricted
    {
        SplitSampleCodeOption co(3, K, CodeOptionSet::Restricted);
        assert(co.length == 2);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption co(4, K, CodeOptionSet::Restricted);
        assert(co.length == 2);
    }
    {
        SplitSampleCodeOption co(5, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(6, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(7, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(8, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(9, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(10, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(11, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(12, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(13, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(14, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(15, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(16, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

template <uint8_t K>
void testK2_5()
{
    // Basic
    {
        SplitSampleCodeOption co(1, K, CodeOptionSet::Basic);
        assert(co.length == 3);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption co(2, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(3, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(4, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(5, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(6, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(7, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(8, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(9, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(10, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(11, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(12, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(13, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(14, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(15, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(16, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }

    // Restricted
    {
        SplitSampleCodeOption co(5, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption co(6, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(7, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(8, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption co(9, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(10, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(11, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(12, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(13, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(14, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(15, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(16, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

template <size_t K>
void testK6_13()
{
    // Basic
    {
        SplitSampleCodeOption co(9, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(10, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(11, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(12, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(13, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(14, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(15, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(16, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }

    // Restricted
    {
        SplitSampleCodeOption co(9, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption co(10, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(11, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(12, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(13, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(14, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(15, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(16, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

template <size_t K>
void testK14_29()
{
    // Basic
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    // Restricted
    {
        SplitSampleCodeOption co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

int main()
{
    try {
        testK1<1>();
        testK2_5<2>();
        testK2_5<3>();
        testK2_5<4>();
        testK2_5<5>();
        testK6_13<6>();
        testK6_13<7>();
        testK6_13<8>();
        testK6_13<9>();
        testK6_13<10>();
        testK6_13<11>();
        testK6_13<12>();
        testK6_13<13>();
        testK14_29<14>();
        testK14_29<15>();
        testK14_29<16>();
        testK14_29<17>();
        testK14_29<18>();
        testK14_29<19>();
        testK14_29<20>();
        testK14_29<21>();
        testK14_29<22>();
        testK14_29<23>();
        testK14_29<24>();
        testK14_29<25>();
        testK14_29<26>();
        testK14_29<27>();
        testK14_29<28>();
        testK14_29<29>();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
