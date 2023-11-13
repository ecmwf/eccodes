#include "GribBits.h"

#include <cassert>

namespace eccodes::accessor {


void gribSetBitOn(AccessorDataPointer p, long& bitp)
{
    p += bitp / 8;
    *p |= (1u << (7 - (bitp % 8)));
    bitp++;
}

void gribSetBitsOn(AccessorDataPointer p, long& bitp, long nbits)
{
    int i;
    for (i = 0; i < nbits; i++) {
        gribSetBitOn(p, bitp);
    }
}

void gribSetBitOff(AccessorDataPointer p, long& bitp)
{
    p += bitp / 8;
    *p &= ~(1u << (7 - (bitp % 8)));
    bitp++;
}

void gribSetBit(AccessorDataPointer p, long bitp, int val)
{
    if (val == 0)
        gribSetBitOff(p, bitp);
    else
        gribSetBitOn(p, bitp);
}

}
