#include <U128.h>

#include <cassert>


void test_subtraction()
{
    {
        UInt128 a{0, 0};
        UInt128 ref{0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
        assert(a - 1 == ref);
    }

    {
        UInt128 a{1, 0};
        UInt128 ref{0, 0xFFFFFFFFFFFFFFFF};
        assert(a - 1 == ref);
    }
    {
        UInt128 a{0, 0xFFFFFFFFFFFFFFFF};
        UInt128 ref{0, 0xFFFFFFFFFFFFFFFE};
        assert(a - 1 == ref);
    }
}

void test_addition()
{
    {
        UInt128 a{0, 0};
        UInt128 ref{0, 1};
        assert(a + 1 == ref);
    }
    {
        UInt128 a{0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
        UInt128 ref{0, 0};
        assert(a + 1 == ref);
    }
    {
        UInt128 a{0, 0xFFFFFFFFFFFFFFFF};
        UInt128 ref{1, 0};
        assert(a + 1 == ref);
    }
}

int main(int argc, char* argv[])
{
    test_subtraction();
    test_addition();
    return 0;
}
