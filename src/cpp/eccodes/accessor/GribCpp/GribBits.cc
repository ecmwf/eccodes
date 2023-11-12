#include "GribBits.h"

#include <cassert>

namespace eccodes::accessor {


void gribSetBitOn(std::vector<unsigned char>& p, long& bitp)
{
    long index = bitp / 8;
    p[index] |= (1u << (7 - ((bitp) % 8)));
    bitp++;
}

void gribSetBitsOn(std::vector<unsigned char>& p, long& bitp, long nbits)
{
    int i;
    for (i = 0; i < nbits; i++) {
        gribSetBitOn(p, bitp);
    }
}

void gribSetBitOff(std::vector<unsigned char>& p, long& bitp)
{
    long index = bitp / 8;
    p[index] &= ~(1u << (7 - ((bitp) % 8)));
    bitp++;
}

void gribSetBit(std::vector<unsigned char>& p, long bitp, int val)
{
    if (val == 0)
        gribSetBitOff(p, bitp);
    else
        gribSetBitOn(p, bitp);
}

}
