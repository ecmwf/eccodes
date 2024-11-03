#pragma once

eccodes::geo_iterator::Iterator* grib_iterator_factory(grib_handle* h, grib_arguments* args, unsigned long flags, int* error);
