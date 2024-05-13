#include "shared_functions.h"


/* Table of multipliers to convert step units to minutes */
static const double u2m[] = {
    1,            /* index 0:  minutes  */
    60,           /* index 1:  hour     */
    24 * 60,      /* index 2:  day      */
    24 * 60 * 30, /* index 3:  month    */
    -1,           /* index 4:  year     */
    -1,           /* index 5:  decade   */
    -1,           /* index 6:  30 years */
    -1,           /* index 7:  century  */
    -1,           /* index 8:  RESERVED */
    -1,           /* index 9:  RESERVED */
    3 * 60,       /* index 10: 3 hours  */
    6 * 60,       /* index 11: 6 hours  */
    12 * 60,      /* index 12: 12 hours */
    1 / 60.0,     /* index 13: seconds  */
    15,           /* index 14: 15 mins  */
    30            /* index 15: 30 mins  */
};


void grib_set_bit_on(unsigned char* p, long* bitp){
    unsigned char o = 1;
    p += (*bitp >> 3);
    o <<= 7 - ((*bitp) % 8);
    *p |= o;
    (*bitp) += 1;
}

long convert_to_minutes(long step, long stepUnits)
{
    double result = 0;
    if (stepUnits == 0)
        return step; /* unit=minutes so no change */
    if (stepUnits == 1)
        return step * 60; /* unit=hours */
    if (stepUnits == 13)
        return step / 60; /* unit=seconds */
    /* Assert( stepUnits < sizeof(u2m)/sizeof(u2m[0]) ); */

    result = step * u2m[stepUnits];
    return (long)result;
}

