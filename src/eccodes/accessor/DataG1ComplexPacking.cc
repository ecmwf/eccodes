/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataG1ComplexPacking.h"

eccodes::accessor::DataG1ComplexPacking _grib_accessor_data_g1complex_packing;
eccodes::Accessor* grib_accessor_data_g1complex_packing = &_grib_accessor_data_g1complex_packing;

namespace eccodes::accessor
{

void DataG1ComplexPacking::init(const long v, grib_arguments* args)
{
    DataComplexPacking::init(v, args);
    half_byte_    = args->get_name(get_enclosing_handle(), carg_++);
    N_            = args->get_name(get_enclosing_handle(), carg_++);
    packingType_  = args->get_name(get_enclosing_handle(), carg_++);
    ieee_packing_ = args->get_name(get_enclosing_handle(), carg_++);
    precision_    = args->get_name(get_enclosing_handle(), carg_++);
    edition_      = 1;
    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
}

int DataG1ComplexPacking::pack_double(const double* val, size_t* len)
{
    int ret             = GRIB_SUCCESS;
    long seclen         = 0;
    long sub_j          = 0;
    long sub_k          = 0;
    long sub_m          = 0;
    long n              = 0;
    long half_byte      = 0;
    long bits_per_value = 0;
    size_t buflen       = 0;

    if (*len == 0)
        return GRIB_NO_VALUES;

    //     /* TODO(masn): spectral_ieee does not work */
    //     if (c->ieee_packing && ieee_packing_ ) {
    //         grib_handle* h       = get_enclosing_handle();
    //         grib_context* c      = context_ ;
    //         char* packingType_s  = NULL;
    //         char* ieee_packing_s = NULL;
    //         long precision = c->ieee_packing == 32 ? 1 : 2;
    //         size_t lenstr  = strlen(ieee_packing_ );

    //         packingType_s  = grib_context_strdup(c, packingType_ );
    //         ieee_packing_s = grib_context_strdup(c, ieee_packing_ );
    //         precision_s    = grib_context_strdup(c, precision_ );

    //         grib_set_string(h, packingType_s, ieee_packing_s, &lenstr);
    //         grib_set_long(h, precision_s, precision);

    //         grib_context_free(c, packingType_s);
    //         grib_context_free(c, ieee_packing_s);
    //         grib_context_free(c, precision_s);
    //         return grib_set_double_array(h, "values", val, *len);
    //     }

    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_j_, &sub_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_k_, &sub_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_m_, &sub_m)) != GRIB_SUCCESS)
        return ret;

    dirty_ = 1;

    ECCODES_ASSERT((sub_j == sub_k) && (sub_m == sub_j));

    ret = DataComplexPacking::pack_double(val, len);

    if (ret == GRIB_SUCCESS) {
        n = offset_ + 4 * ((sub_k + 1) * (sub_k + 2));

        /*     Octet number starts from beginning of message but shouldn't     */
        if ((ret = grib_set_long_internal(get_enclosing_handle(), N_, n)) != GRIB_SUCCESS)
            return ret;

        // ret = grib_get_long_internal(get_enclosing_handle(), offsetsection_ , &offsetsection);
        // if (ret != GRIB_SUCCESS)
        //     return ret;
        // if ((ret = grib_set_long_internal(get_enclosing_handle(), N_ , n - offsetsection)) != GRIB_SUCCESS)
        //     return ret;

        ret = grib_get_long_internal(get_enclosing_handle(), bits_per_value_, &bits_per_value);
        if (ret != GRIB_SUCCESS)
            return ret;

        ret = grib_get_long_internal(get_enclosing_handle(), seclen_, &seclen);
        if (ret != GRIB_SUCCESS)
            return ret;

        buflen    = 32 * (sub_k + 1) * (sub_k + 2) + (*len - (sub_k + 1) * (sub_k + 2)) * bits_per_value + 18 * 8;
        half_byte = seclen * 8 - buflen;
        if (context_->debug == -1) {
            fprintf(stderr, "ECCODES DEBUG: half_byte=%ld\n", half_byte);
        }

        ret = grib_set_long_internal(get_enclosing_handle(), half_byte_, half_byte);
        if (ret != GRIB_SUCCESS)
            return ret;
    }
    return ret;
}

}  // namespace eccodes::accessor
