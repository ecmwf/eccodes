#pragma once


class HEALPix {
public:
    explicit HEALPix(int Nside);

    int size() const { return 12 * Nside_ * Nside_; }
    int nside() const { return Nside_; }

    int nest_to_ring(int) const;
    int ring_to_nest(int) const;

private:
    const int Nside_;  // up to 2^13
    const int Npix_;
    const int Ncap_;
    const int k_;
};
