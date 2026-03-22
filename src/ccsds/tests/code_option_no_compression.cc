#include <cassert>

#include "CodeOption.h"


int main() {
    // Basic
    {
        NoCompressionCodeOption co(1, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(2, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(3, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(4, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(5, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(6, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(7, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(8, CodeOptionSet::Basic);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(9, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(10, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(11, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(12, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(13, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(14, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(15, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(16, CodeOptionSet::Basic);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(17, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(18, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(19, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(20, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(21, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(22, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(23, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(24, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(25, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(26, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(27, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(28, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(29, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(30, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(31, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(32, CodeOptionSet::Basic);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }

    // Restricted
    {
        NoCompressionCodeOption co(1, CodeOptionSet::Restricted);
        assert(co.value == 0b1);
        assert(co.length == 1);
    }
    {
        NoCompressionCodeOption co(2, CodeOptionSet::Restricted);
        assert(co.value == 0b1);
        assert(co.length == 1);
    }
    {
        NoCompressionCodeOption co(3, CodeOptionSet::Restricted);
        assert(co.value == 0b11);
        assert(co.length == 2);
    }
    {
        NoCompressionCodeOption co(4, CodeOptionSet::Restricted);
        assert(co.value == 0b11);
        assert(co.length == 2);
    }
    {
        NoCompressionCodeOption co(5, CodeOptionSet::Restricted);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(6, CodeOptionSet::Restricted);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(7, CodeOptionSet::Restricted);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(8, CodeOptionSet::Restricted);
        assert(co.value == 0b111);
        assert(co.length == 3);
    }
    {
        NoCompressionCodeOption co(9, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(10, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(11, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(12, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(13, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(14, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(15, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(16, CodeOptionSet::Restricted);
        assert(co.value == 0b1111);
        assert(co.length == 4);
    }
    {
        NoCompressionCodeOption co(17, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(18, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(19, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(20, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(21, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(22, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(23, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(24, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(25, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(26, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(27, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(28, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(29, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(30, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(31, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }
    {
        NoCompressionCodeOption co(32, CodeOptionSet::Restricted);
        assert(co.value == 0b11111);
        assert(co.length == 5);
    }

    return 0;
}
