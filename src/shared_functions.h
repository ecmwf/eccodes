#ifndef eccodes_shared_functions_h
#define eccodes_shared_functions_h

#include "grib_api_internal.h"

void grib_set_bit_on(unsigned char*, long*);
long convert_to_minutes(long step, long stepUnits);

#endif // eccodes_shared_functions_h
