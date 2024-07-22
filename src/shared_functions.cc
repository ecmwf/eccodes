#include "shared_functions.h"

void grib_set_bit_on(unsigned char* p, long* bitp){
    unsigned char o = 1;
    p += (*bitp >> 3);
    o <<= 7 - ((*bitp) % 8);
    *p |= o;
    (*bitp) += 1;
}
