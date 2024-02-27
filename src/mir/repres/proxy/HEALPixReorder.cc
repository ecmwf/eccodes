#include "HEALPix.h"

#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <tuple>


namespace {


struct CodecFijNest {
    static constexpr uint64_t __masks[] = {0x00000000ffffffff, 0x0000ffff0000ffff, 0x00ff00ff00ff00ff,
                                           0x0f0f0f0f0f0f0f0f, 0x3333333333333333, 0x5555555555555555};

    inline static int nest_encode_bits(int n) {
        auto b = static_cast<uint64_t>(n) & __masks[0];
        b      = (b ^ (b << 16)) & __masks[1];
        b      = (b ^ (b << 8)) & __masks[2];
        b      = (b ^ (b << 4)) & __masks[3];
        b      = (b ^ (b << 2)) & __masks[4];
        b      = (b ^ (b << 1)) & __masks[5];
        return static_cast<int>(b);
    }

    inline static int nest_decode_bits(int n) {
        auto b = static_cast<uint64_t>(n) & __masks[5];
        b      = (b ^ (b >> 1)) & __masks[4];
        b      = (b ^ (b >> 2)) & __masks[3];
        b      = (b ^ (b >> 4)) & __masks[2];
        b      = (b ^ (b >> 8)) & __masks[1];
        b      = (b ^ (b >> 16)) & __masks[0];
        return static_cast<int>(b);
    }

    static std::tuple<int, int, int> nest_to_fij(int n, int k) {
        assert(0 <= n);
        auto f = n >> (2 * k);    // f = n / (Nside * Nside)
        n &= (1 << (2 * k)) - 1;  // n = n % (Nside * Nside)
        auto i = nest_decode_bits(n);
        auto j = nest_decode_bits(n >> 1);
        return {f, i, j};
    }

    static int fij_to_nest(int f, int i, int j, int k) {
        return (f << (2 * k)) + nest_encode_bits(i) + (nest_encode_bits(j) << 1);
    }
};


inline int sqrt(int n) {
    return static_cast<int>(std::sqrt(static_cast<double>(n) + 0.5));
}


// for division result within [0; 3]
inline int div_03(int a, int b) {
    int t = (a >= (b << 1)) ? 1 : 0;
    a -= t * (b << 1);
    return (t << 1) + (a >= b ? 1 : 0);
}


inline bool is_power_of_2(int n) {
    return std::bitset<sizeof(int) * 8>(n).count() == 1;
}


inline int pll(int f) {
    constexpr int __pll[] = {1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};
    return __pll[f];
}


}  // unnamed namespace


HEALPix::HEALPix(int Nside) :
    Nside_(Nside),
    Npix_(size()),
    Ncap_((Nside * (Nside - 1)) << 1),
    k_(is_power_of_2(Nside_) ? static_cast<int>(std::log2(Nside)) : -1) {
    assert(0 <= k_);  // (specific to nested ordering)
    assert(0 < Nside_);
}


int HEALPix::ring_to_nest(int r) const {
    auto to_nest = [&](int f,      //!< base pixel index
                       int ring,   //!< 1-based ring number
                       int Nring,  //!< number of pixels in ring
                       int phi,    //!< index in longitude
                       int shift   //!< if ring's first pixel is not at phi=0
                       ) -> int {
        int r = ((2 + (f >> 2)) << k_) - ring - 1;
        int p = 2 * phi - pll(f) * Nring - shift - 1;
        if (p >= 2 * Nside_) {
            p -= 8 * Nside_;
        }

        int i = (r + p) >> 1;
        int j = (r - p) >> 1;

        assert(f < 12 && i < Nside_ && j < Nside_);
        return CodecFijNest::fij_to_nest(f, i, j, k_);
    };

    if (r < Ncap_) {
        // North polar cap
        int Nring = (1 + sqrt(2 * r + 1)) >> 1;
        int phi   = 1 + r - 2 * Nring * (Nring - 1);
        int r     = Nring;
        int f     = div_03(phi - 1, Nring);

        return to_nest(f, r, Nring, phi, 0);
    }

    if (Npix_ - Ncap_ <= r) {
        // South polar cap
        int Nring = (1 + sqrt(2 * Npix_ - 2 * r - 1)) >> 1;
        int phi   = 1 + r + 2 * Nring * (Nring - 1) + 4 * Nring - Npix_;
        int ring  = 4 * Nside_ - Nring;  // (from South pole)
        int f     = div_03(phi - 1, Nring) + 8;

        return to_nest(f, ring, Nring, phi, 0);
    }

    // Equatorial belt
    int ip  = r - Ncap_;
    int tmp = ip >> (k_ + 2);

    int Nring = Nside_;
    int phi   = ip - tmp * 4 * Nside_ + 1;
    int ring  = tmp + Nside_;

    int ifm = 1 + ((phi - 1 - ((1 + tmp) >> 1)) >> k_);
    int ifp = 1 + ((phi - 1 - ((1 - tmp + 2 * Nside_) >> 1)) >> k_);
    int f   = (ifp == ifm) ? (ifp | 4) : ((ifp < ifm) ? ifp : (ifm + 8));

    return to_nest(f, ring, Nring, phi, ring & 1);
}


int HEALPix::nest_to_ring(int n) const {
    auto [f, i, j] = CodecFijNest::nest_to_fij(n, k_);
    assert(f < 12 && i < Nside_ && j < Nside_);

    auto to_ring_local = [&](int f, int i, int j,
                             int Nring,  //!< number of pixels in ring
                             int shift   //!< if ring's first pixel is/is not at phi=0
                             ) -> int {
        Nring >>= 2;
        int r = (pll(f) * Nring + i - j + 1 + shift) / 2 - 1;
        assert(r < 4 * Nring);

        return r < 0 ? r + 4 * Nside_ : r;
    };

    const int ring = ((f >> 2) + 2) * Nside_ - i - j - 1;  // 1-based ring number
    if (ring < Nside_) {
        // North polar cap
        int Nring = 4 * ring;
        int r0    = 2 * ring * (ring - 1);  // index of first ring pixel (ring numbering)

        return r0 + to_ring_local(f, i, j, Nring, 0);
    }

    if (ring < 3 * Nside_) {
        // South polar cap
        int Nring = 4 * Nside_;
        int r0    = Ncap_ + (ring - Nside_) * Nring;  // index of first ring pixel (ring numbering)
        int shift = (ring - Nside_) & 1;

        return r0 + to_ring_local(f, i, j, Nring, shift);
    }

    // Equatorial belt
    int N     = 4 * Nside_ - ring;
    int Nring = 4 * N;
    int r0    = Npix_ - 2 * N * (N + 1);  // index of first ring pixel (ring numbering)

    return r0 + to_ring_local(f, i, j, Nring, 0);
}
