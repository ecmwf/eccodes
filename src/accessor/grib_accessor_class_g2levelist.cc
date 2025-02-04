/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_g2levelist.h"

grib_accessor_g2levelist_t _grib_accessor_g2levelist{};
grib_accessor* grib_accessor_g2levelist = &_grib_accessor_g2levelist;

void grib_accessor_g2levelist_t::init(const long l, grib_arguments* args)
{
    grib_accessor_long_t::init(l, args);
    grib_handle* hand = grib_handle_of_accessor(this);
    int n = 0;
    levelAccessorName_ = args->get_name(hand, n++);

    pressure_units_ = args->get_name(hand, n++);
    levtype_        = args->get_name(hand, n++);
    typeOfLevel_    = args->get_name(hand, n++);
}

// grib_accessor* grib_accessor_g2levelist_t::get_level_accessor()
// {
//     if (!levelAccessor_) {
//         levelAccessor_ = grib_find_accessor(grib_handle_of_accessor(this), levelAccessorName_);
//     }
//     return levelAccessor_;
// }

bool grib_accessor_g2levelist_t::exists()
{
    int err = 0;
    grib_handle* h = grib_handle_of_accessor(this);

    char typeOfLevel[100];
    size_t size = sizeof(typeOfLevel);
    err = grib_get_string(h, this->typeOfLevel_, typeOfLevel, &size);
    if (err) return false;
    if (STR_EQUAL(typeOfLevel, "surface")) {
        return false;
    }

    char levtype[100];
    size = sizeof(levtype);
    err = grib_get_string(h, this->levtype_, levtype, &size);
    if (err) return false;
    if (STR_EQUAL(levtype, "sfc") || STR_EQUAL(levtype, "o2d")) {
        return false;
    }
    return true;
}

int grib_accessor_g2levelist_t::pack_long(const long* val, size_t* len)
{
    if (!exists()) {
        return GRIB_NOT_FOUND;
    }
    grib_handle* h = grib_handle_of_accessor(this);
    int err = grib_set_long(h, this->levelAccessorName_, *val);
    return err;
}

int grib_accessor_g2levelist_t::unpack_long(long* val, size_t* len)
{
    if (!exists()) {
        return GRIB_NOT_FOUND;
    }

    int err = 0;
    grib_handle* h = grib_handle_of_accessor(this);

    long level = 0;
    err = grib_get_long_internal(h, this->levelAccessorName_, &level);
    if (err) return err;
    *val = level;
    
    return GRIB_SUCCESS;
}

int grib_accessor_g2levelist_t::pack_double(const double* val, size_t* len)
{
    if (!exists()) {
        return GRIB_NOT_FOUND;
    }

    grib_handle* h = grib_handle_of_accessor(this);
    int err = 0;
    double dValue = *val;
    const long lValue = (long)dValue;

    if (dValue == lValue) {  // input is a whole number; process it as an integer
        return pack_long(&lValue, len);
    }

    err = grib_set_double(h, this->levelAccessorName_, *val);
    return err;
}

int grib_accessor_g2levelist_t::unpack_double(double* val, size_t* len)
{
    if (!exists()) {
        return GRIB_NOT_FOUND;
    }

    grib_handle* h = grib_handle_of_accessor(this);
    double level = 0;
    int err = grib_get_double_internal(h, this->levelAccessorName_, &level);
    if (err) return err;
    *val = level;
    
    return GRIB_SUCCESS;
}

// int grib_accessor_g2levelist_t::unpack_double(double* val, size_t* len)
// {
//     int ret           = 0;
//     grib_handle* hand = grib_handle_of_accessor(this);

//     long type_first         = 0;
//     long scale_first        = 0;
//     long value_first        = 0;
//     char pressure_units[10] = {0,};
//     size_t pressure_units_len = 10;
//     bool tigge                = is_tigge(hand);

//     double v;

//     if ((ret = grib_get_long_internal(hand, type_first_, &type_first)) != GRIB_SUCCESS)
//         return ret;
//     if ((ret = grib_get_long_internal(hand, scale_first_, &scale_first)) != GRIB_SUCCESS)
//         return ret;
//     if ((ret = grib_get_long_internal(hand, value_first_, &value_first)) != GRIB_SUCCESS)
//         return ret;
//     if ((ret = grib_get_string_internal(hand, pressure_units_, pressure_units, &pressure_units_len)) != GRIB_SUCCESS)
//         return ret;

//     if (value_first == GRIB_MISSING_LONG) {
//         *val = 0;
//         return GRIB_SUCCESS;
//     }
//     // value = value_first * 10 ^ -scale_first

//     if (*len < 1)
//         return GRIB_WRONG_ARRAY_SIZE;

//     v = value_first;

//     if (scale_first != GRIB_MISSING_LONG) {
//         // GRIB-637, ECC-1081: Potential vorticity surface
//         if (type_first == 109) {
//             if (tigge) {
//                 scale_first -= 6;  // TIGGE data follows different rules
//             }
//             else {
//                 scale_first -= 9;
//             }
//         }

//         while (scale_first < 0 && v != 0) {
//             v *= 10.0;
//             scale_first++;
//         }
//         while (scale_first > 0 && v != 0) {
//             v /= 10.0;
//             scale_first--;
//         }
//     }

//     switch (type_first) {
//         case 100:  // Isobaric surface (Pa)
//             if (!strcmp(pressure_units, "hPa")) {
//                 long x = v / 100.0;  // 1 hPa = 100 Pa
//                 if (scale_first == 0 && x == 0) {
//                     // Switch to Pa instead of hPa as the value is less than a hectoPascal
//                     char pa[]  = "Pa";
//                     size_t lpa = strlen(pa);
//                     if ((ret = grib_set_string_internal(hand, pressure_units_, pa, &lpa)) != GRIB_SUCCESS)
//                         return ret;
//                 }
//                 else {
//                     v = x;
//                 }
//             }
//             break;
//     }
//     *val = v;
//     return GRIB_SUCCESS;
// }

//---------------------
// int grib_accessor_g2levelist_t::pack_double(const double* val, size_t* len)
// {
//     grib_handle* hand  = grib_handle_of_accessor(this);
//     int ret            = 0;
//     double value_first = *val;
//     // long scale_first           = 0;
//     long type_first         = 0;
//     char pressure_units[10] = {0,};
//     size_t pressure_units_len = 10;
//     const long lval           = (long)value_first;

//     if (value_first == lval) {  // input is a whole number; process it as an integer
//         return pack_long(&lval, len);
//     }

//     if (*len != 1)
//         return GRIB_WRONG_ARRAY_SIZE;

//     if ((ret = grib_get_long_internal(hand, type_first_, &type_first)) != GRIB_SUCCESS)
//         return ret;

//     if ((ret = grib_get_string_internal(hand, pressure_units_, pressure_units, &pressure_units_len)) != GRIB_SUCCESS)
//         return ret;

//     switch (type_first) {
//         case 100:  // Pa
//             if (!strcmp(pressure_units, "hPa"))
//                 value_first *= 100;
//             break;

//         default:
//             break;
//     }

//     // final = scaled_value * 10 ^ -scale_factor

//     // scale_first = 2;
//     // value_first *= 100;
//     // value_first = value_first + 0.5; //round up

//     // TODO(masn): These maxima should come from the respective accessors
//     const int64_t scaled_value_max = (1ULL << 32) - 1;  // scaledValueOf*FixedSurface is 4 octets
//     const int64_t scale_factor_max = (1ULL << 8) - 1;   // scaleFactorOf*FixedSurface is 1 octet
//     int64_t lscaled_value = 0, lscale_factor = 0;

//     ret = compute_scaled_value_and_scale_factor(value_first, scaled_value_max, scale_factor_max, &lscaled_value, &lscale_factor);
//     if (ret) {
//         grib_context_log(context_, GRIB_LOG_ERROR, "Key %s (unpack_double): Failed to compute %s and %s from %g",
//                          name_, scale_first_, value_first_, value_first);
//         return ret;
//     }

//     if (type_first > 9) {
//         if ((ret = grib_set_long_internal(hand, scale_first_, (long)lscale_factor)) != GRIB_SUCCESS)
//             return ret;
//         if ((ret = grib_set_long_internal(hand, value_first_, (long)lscaled_value)) != GRIB_SUCCESS)
//             return ret;
//     }

//     return GRIB_SUCCESS;
// }

//int grib_accessor_g2levelist_t::pack_long(const long* val, size_t* len)//
//{
  //  return GRIB_NOT_IMPLEMENTED;

    // int ret                 = 0;
    // long value_first        = *val;
    // long scale_first        = 0;
    // long type_first         = 0;
    // char pressure_units[10] = {0,};
    // size_t pressure_units_len = 10;

    // grib_handle* hand          = grib_handle_of_accessor(this);
    // int change_scale_and_value = 1;
    // bool tigge                 = is_tigge(hand);

    // if (*len != 1)
    //     return GRIB_WRONG_ARRAY_SIZE;

    // if ((ret = grib_get_long_internal(hand, type_first_, &type_first)) != GRIB_SUCCESS)
    //     return ret;

    // if ((ret = grib_get_string_internal(hand, pressure_units_, pressure_units, &pressure_units_len)) != GRIB_SUCCESS)
    //     return ret;

    // switch (type_first) {
    //     case 100:  // Pa
    //         scale_first = 0;
    //         if (!strcmp(pressure_units, "hPa"))
    //             value_first *= 100;
    //         break;
    //     case 109:  // Potential vorticity surface (See ECC-1081)
    //         if (!tigge) {
    //             scale_first = 9;
    //         }
    //         else {
    //             scale_first = 6;  // TIGGE data follows different rules
    //         }
    //         break;

    //     default:
    //         break;
    // }

    // // ECC-530:
    // // The pack_long function can get called when key "typeOfSecondFixedSurface" is
    // // changed (via the trigger rule in the definitions). That can have an undesired
    // // side-effect that it sets the scale factor and scaled value keys
    // // (e.g. scaleFactorOfFirstFixedSurface, scaledValueOfFirstFixedSurface)
    // // overwriting their previous values.
    // // In this scenario we do not want to change the scale/value.
    // // However when the user directly sets the level or when we are changing edition, then
    // // we do want to change the scale/value.
    // // if (hand->loader && hand->loader->changing_edition==0) {
    // //    change_scale_and_value = 0;
    // // }

    // if (change_scale_and_value) {
    //     if (type_first > 9) {
    //         if ((ret = grib_set_long_internal(hand, scale_first_, scale_first)) != GRIB_SUCCESS)
    //             return ret;
    //         if ((ret = grib_set_long_internal(hand, value_first_, value_first)) != GRIB_SUCCESS)
    //             return ret;
    //     }
    // }

    // return GRIB_SUCCESS;
//}

int grib_accessor_g2levelist_t::is_missing()
{
    // grib_handle* hand = grib_handle_of_accessor(this);
    // int err           = 0;

    // int ret = grib_is_missing(hand, scale_first_, &err) +
    //           grib_is_missing(hand, value_first_, &err);
    return 0;
}
