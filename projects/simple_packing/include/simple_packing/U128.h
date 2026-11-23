
#pragma once

#include <cstdint>
#include <type_traits>

struct UInt128
{
    using u64 = std::uint64_t;
    using u32 = std::uint32_t;

    u64 lo{ 0 };
    u64 hi{ 0 };

    // Constructors
    constexpr UInt128() = default;
    constexpr UInt128(u64 low) : lo(low), hi(0) {}
    constexpr UInt128(u64 high, u64 low) : lo(low), hi(high) {}

    // Comparison
    friend constexpr bool operator==(const UInt128& a, const UInt128& b)
    {
        return a.hi == b.hi && a.lo == b.lo;
    }
    friend constexpr bool operator!=(const UInt128& a, const UInt128& b) { return !(a == b); }

    // Unary
    friend constexpr UInt128 operator~(const UInt128& x)
    {
        return UInt128(~x.hi, ~x.lo);
    }

    // Bitwise
    friend constexpr UInt128 operator&(const UInt128& a, const UInt128& b) { return UInt128(a.hi & b.hi, a.lo & b.lo); }
    friend constexpr UInt128 operator|(const UInt128& a, const UInt128& b) { return UInt128(a.hi | b.hi, a.lo | b.lo); }

    UInt128& operator|=(const UInt128& o)
    {
        hi |= o.hi;
        lo |= o.lo;
        return *this;
    }

    // Shifts
    friend constexpr UInt128 operator<<(const UInt128& x, unsigned s)
    {
        if (s == 0) return x;
        if (s >= 128) return UInt128(0, 0);
        if (s >= 64) {
            return UInt128(x.lo << (s - 64), 0);
        }
        else {
            u64 new_hi = (x.hi << s) | (x.lo >> (64 - s));
            u64 new_lo = x.lo << s;
            return UInt128(new_hi, new_lo);
        }
    }
    friend constexpr UInt128 operator>>(const UInt128& x, unsigned s)
    {
        if (s == 0) return x;
        if (s >= 128) return UInt128(0, 0);
        if (s >= 64) {
            return UInt128(0, x.hi >> (s - 64));
        }
        else {
            u64 new_lo = (x.lo >> s) | (x.hi << (64 - s));
            u64 new_hi = x.hi >> s;
            return UInt128(new_hi, new_lo);
        }
    }

    // Addition / subtraction
    friend constexpr UInt128 operator+(const UInt128& a, const UInt128& b)
    {
        UInt128 r;
        r.lo = a.lo + b.lo;
        r.hi = a.hi + b.hi + (r.lo < a.lo);
        return r;
    }
    friend constexpr UInt128 operator-(const UInt128& a, const UInt128& b)
    {
        UInt128 r;
        r.lo = a.lo - b.lo;
        r.hi = a.hi - b.hi - (a.lo < b.lo);
        return r;
    }

    UInt128& operator<<=(unsigned s)
    {
        *this = *this << s;
        return *this;
    }

    template <class T, class = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value && (sizeof(T) <= sizeof(std::uint64_t))>>
    explicit constexpr operator T() const noexcept
    {
        // By default, we *truncate* to the low bits (like builtin casts).
        return static_cast<T>(lo);
    }
};
