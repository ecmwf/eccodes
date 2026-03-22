#include <cassert>

#include "CodeOption.h"


template <size_t K>
void testK1() {
    // Basic
    {
        SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Basic);
        assert(co.length == 3);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }

    // Restricted
    // {
    //     SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    {
        SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Restricted);
        assert(co.length == 2);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Restricted);
        assert(co.length == 2);
    }
    {
        SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

template <uint8_t K>
void testK2_5() {
    // Basic
    {
        SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Basic);
        assert(co.length == 3);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Basic);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }

    // Restricted
    // {
    //     SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    {
        SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Restricted);
        assert(co.length == 3);
    }
    {
        SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

template <size_t K>
void testK6_13() {
    // Basic
    // {
    //     SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    {
        SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Basic);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }

    // Restricted
    // {
    //     SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    {
        SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Restricted);
        assert(co.length == 4);
    }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

template <size_t K>
void testK14_29() {
    // Basic
    // {
    //     SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Basic);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Basic);
    //     assert(co.length == 4);
    // }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Basic);
        assert(co.length == 5);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Basic);
        assert(co.length == 5);
    }
    // Restricted
    // {
    //     SplitSampleCodeOption<uint32_t> co(1, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(2, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(3, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(4, K, CodeOptionSet::Restricted);
    //     assert(co.length == 2);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(5, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(6, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(7, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(8, K, CodeOptionSet::Restricted);
    //     assert(co.length == 3);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(9, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(10, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(11, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(12, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(13, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(14, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(15, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    // {
    //     SplitSampleCodeOption<uint32_t> co(16, K, CodeOptionSet::Restricted);
    //     assert(co.length == 4);
    // }
    {
        SplitSampleCodeOption<uint32_t> co(17, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
        assert(co.value == K + 1);
    }
    {
        SplitSampleCodeOption<uint32_t> co(18, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(19, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(20, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(21, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(22, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(23, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(24, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(25, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(26, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(27, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(28, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(29, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(30, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(31, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
    {
        SplitSampleCodeOption<uint32_t> co(32, K, CodeOptionSet::Restricted);
        assert(co.length == 5);
    }
}

int main() {
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
