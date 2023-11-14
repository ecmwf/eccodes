#pragma once

// C++ implementation of the existing grib_util.cc helpers
#include "AccessorBuffer.h"
#include <string>

namespace eccodes::accessor {

std::string gribContextFullDefsPath(std::string basename);

int gribContextGetHandleFileCount();

int gribContextGetHandleTotalCount();

//void* grib_context_buffer_malloc_clear(const grib_context* c, size_t size);
AccessorDataBuffer gribcontextBufferMallocClear(size_t size);
}
