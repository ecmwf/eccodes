// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Span::operator[] must check the index in debug builds (the assert came after the return and
// was never executed). This test accesses an element past the end: the assertion must abort.

#include <csignal>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <vector>

#ifdef NDEBUG
    #error "span_bounds must be compiled with assertions enabled"
#endif

#include "spaec/detail/Span.h"

using namespace spaec::detail;

static void onAbort(int)
{
    _exit(0);  // the out of range access was detected
}

int main()
{
    (void)std::signal(SIGABRT, onAbort);
    std::vector<uint8_t> data = { 1, 2, 3, 4 };
    Span<uint8_t> span(data);
    volatile uint8_t value = span[data.size()];
    std::cerr << "FAIL: out of range access was not detected: " << static_cast<int>(value) << '\n';
    return 1;
}
