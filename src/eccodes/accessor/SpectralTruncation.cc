/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "SpectralTruncation.h"

eccodes::AccessorBuilder<eccodes::accessor::SpectralTruncation> _grib_accessor_spectral_truncation_builder{};

namespace eccodes::accessor
{

void SpectralTruncation::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n = 0;

    J_ = c->get_name(get_enclosing_handle(), n++);
    K_ = c->get_name(get_enclosing_handle(), n++);
    M_ = c->get_name(get_enclosing_handle(), n++);
    T_ = c->get_name(get_enclosing_handle(), n++);

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int SpectralTruncation::unpack_long(long* val, size_t* len)
{
    int ret = 0;

    long J, K, M, T, Tc;

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), J_, &J)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), K_, &K)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), M_, &M)) != GRIB_SUCCESS)
        return ret;

    Tc = -1;
    if (J == K && K == M) {
        /* Triangular truncation */
        Tc = (M + 1) * (M + 2);
    }
    if (K == J + M) {
        /* Rhomboidal truncation */
        Tc = 2 * J * M;
    }
    if (J == K && K > M) {
        /* Trapezoidal truncation */
        Tc = M * (2 * J - M);
    }
    *val = Tc;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), T_, &T)) != GRIB_SUCCESS) {
        if (Tc == -1)
            grib_context_log(context_, GRIB_LOG_ERROR,
                             "%s. Spectral Truncation Type Unknown: %s=%ld %s=%ld %s=%ld",
                             name_, J_, J, K_, K, M_, M);
        Tc = 0;
        grib_set_long(get_enclosing_handle(), T_, Tc);
    }
    else {
        if (Tc != -1 && Tc != T)
            grib_set_long(get_enclosing_handle(), T_, Tc);
    }

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}

}  // namespace eccodes::accessor
