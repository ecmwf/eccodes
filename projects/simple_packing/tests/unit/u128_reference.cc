// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Differential unit test of UInt128 against the compiler's unsigned __int128: every operator for
// edge values (0, 1, 2^63, 2^64 - 1, 2^64, 2^127, the maximum and their neighbours) and random
// values, shifts by every count 0..200 (counts of 128 or more give 0), comparisons and the
// conversion to 64 bits. Built only where unsigned __int128 exists.
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "simple_packing/detail/U128.h"

using namespace sp::detail;

#if defined(__SIZEOF_INT128__)
__extension__ using Ref = unsigned __int128;

static int32_t failures = 0;

static UInt128 make(Ref r)
{
    return UInt128(static_cast<uint64_t>(r >> 64), static_cast<uint64_t>(r));
}

static bool same(const UInt128& a, Ref r)
{
    return a.hi == static_cast<uint64_t>(r >> 64) && a.lo == static_cast<uint64_t>(r);
}

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

static void pair(Ref x, Ref y)
{
    const UInt128 a = make(x), b = make(y);
    const std::string n = std::to_string(static_cast<uint64_t>(x >> 64)) + ":" + std::to_string(static_cast<uint64_t>(x)) + " op " +
                          std::to_string(static_cast<uint64_t>(y >> 64)) + ":" + std::to_string(static_cast<uint64_t>(y));
    check(same(a + b, x + y), "add " + n);
    check(same(a - b, x - y), "subtract " + n);
    check(same(a & b, x & y) && same(a | b, x | y) && same(~a, ~x), "bit operations " + n);
    check((a == b) == (x == y) && (a != b) == (x != y), "comparison " + n);
    UInt128 c = a;
    c |= b;
    check(same(c, x | y), "|= " + n);
    check(static_cast<uint64_t>(a) == static_cast<uint64_t>(x), "conversion to 64 bits " + n);
}

static void shifts(Ref x)
{
    for (uint32_t s = 0; s <= 200; ++s) {
        const Ref left = s >= 128 ? Ref(0) : x << s;
        const Ref right = s >= 128 ? Ref(0) : x >> s;
        check(same(make(x) << s, left), "shift left by " + std::to_string(s));
        check(same(make(x) >> s, right), "shift right by " + std::to_string(s));
        UInt128 y = make(x);
        y <<= s;
        check(same(y, left), "<<= " + std::to_string(s));
    }
}

int main()
{
    const Ref max = ~Ref(0);
    std::vector<Ref> values = { 0, 1, 2, (Ref(1) << 63) - 1, Ref(1) << 63, (Ref(1) << 64) - 1, Ref(1) << 64, (Ref(1) << 64) + 1, Ref(1) << 127, max - 1, max };
    std::mt19937_64 rng(19);
    for (int32_t i = 0; i < 40; ++i) {
        values.push_back((Ref(rng()) << 64) | rng());
    }
    for (Ref x : values) {
        shifts(x);
        for (Ref y : values) {
            pair(x, y);
        }
    }
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
#else
int main()
{
    std::cout << "unsigned __int128 not available: test skipped\n";
    return 0;
}
#endif
