#include <cassert>

#include "CodeOption.h"


int main() {
    // Basic
    {
        SecondExtensionCodeOption co(1, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(2, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(3, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(4, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(5, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(6, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(7, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(8, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(9, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(10, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(11, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(12, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(13, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(14, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(15, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(16, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(17, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(18, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(19, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(20, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(21, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(22, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(23, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(24, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(25, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(26, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(27, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(28, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(29, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(30, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(31, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(32, CodeOptionSet::Basic);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }

    // Restricted
    {
        SecondExtensionCodeOption co(1, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 2);
    }
    {
        SecondExtensionCodeOption co(2, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 2);
    }
    {
        SecondExtensionCodeOption co(3, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 3);
    }
    {
        SecondExtensionCodeOption co(4, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 3);
    }
    {
        SecondExtensionCodeOption co(5, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(6, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(7, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(8, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 4);
    }
    {
        SecondExtensionCodeOption co(9, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(10, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(11, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(12, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(13, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(14, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(15, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(16, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 5);
    }
    {
        SecondExtensionCodeOption co(17, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(18, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(19, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(20, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(21, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(22, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(23, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(24, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(25, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(26, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(27, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(28, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(29, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(30, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(31, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
    {
        SecondExtensionCodeOption co(32, CodeOptionSet::Restricted);
        assert(co.value == 0x01);
        assert(co.length == 6);
    }
}



