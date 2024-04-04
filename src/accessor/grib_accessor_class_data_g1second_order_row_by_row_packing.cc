
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_g1second_order_row_by_row_packing.h"

grib_accessor_class_data_g1second_order_row_by_row_packing_t _grib_accessor_class_data_g1second_order_row_by_row_packing{"data_g1second_order_row_by_row_packing"};
grib_accessor_class* grib_accessor_class_data_g1second_order_row_by_row_packing = &_grib_accessor_class_data_g1second_order_row_by_row_packing;


void grib_accessor_class_data_g1second_order_row_by_row_packing_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_class_data_simple_packing_t::init(a, v, args);
    grib_accessor_data_g1second_order_row_by_row_packing_t* self = (grib_accessor_data_g1second_order_row_by_row_packing_t*)a;
    grib_handle* gh                                            = grib_handle_of_accessor(a);

    self->half_byte                       = grib_arguments_get_name(gh, args, self->carg++);
    self->packingType                     = grib_arguments_get_name(gh, args, self->carg++);
    self->ieee_packing                    = grib_arguments_get_name(gh, args, self->carg++);
    self->precision                       = grib_arguments_get_name(gh, args, self->carg++);
    self->widthOfFirstOrderValues         = grib_arguments_get_name(gh, args, self->carg++);
    self->N1                              = grib_arguments_get_name(gh, args, self->carg++);
    self->N2                              = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfGroups                  = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfSecondOrderPackedValues = grib_arguments_get_name(gh, args, self->carg++);
    self->extraValues                     = grib_arguments_get_name(gh, args, self->carg++);
    self->Ni                              = grib_arguments_get_name(gh, args, self->carg++);
    self->Nj                              = grib_arguments_get_name(gh, args, self->carg++);
    self->pl                              = grib_arguments_get_name(gh, args, self->carg++);
    self->jPointsAreConsecutive           = grib_arguments_get_name(gh, args, self->carg++);
    self->groupWidths                     = grib_arguments_get_name(gh, args, self->carg++);
    self->bitmap                          = grib_arguments_get_name(gh, args, self->carg++);
    self->edition                         = 1;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

int grib_accessor_class_data_g1second_order_row_by_row_packing_t::value_count(grib_accessor* a, long* count){
    grib_accessor_data_g1second_order_row_by_row_packing_t* self = (grib_accessor_data_g1second_order_row_by_row_packing_t*)a;
    grib_handle* gh                                            = grib_handle_of_accessor(a);
    long n = 0, i = 0;
    long numberOfRows          = 0;
    long jPointsAreConsecutive = 0;
    long Ni = 0, Nj = 0;
    int bitmapPresent = 0;
    size_t plSize     = 0;
    long* pl          = 0;
    int ret           = 0;
    grib_context* c   = a->context;

    if (self->bitmap)
        bitmapPresent = 1;
    if ((ret = grib_get_long_internal(gh, self->jPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->Ni, &Ni)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->Nj, &Nj)) != GRIB_SUCCESS)
        return ret;
    if (jPointsAreConsecutive) {
        numberOfRows = Ni;
    }
    else {
        numberOfRows = Nj;
    }

    plSize = 0;
    ret    = grib_get_size(gh, self->pl, &plSize);
    if (ret == GRIB_SUCCESS) {
        pl = (long*)grib_context_malloc_clear(a->context, sizeof(long) * plSize);
        if ((ret = grib_get_long_array(gh, self->pl, pl, &plSize)) != GRIB_SUCCESS)
            return ret;
    }
    ret = 0;

    n = 0;
    if (bitmapPresent) {
        long *bitmap, *pbitmap;
        size_t numberOfPoints = 0;

        if (plSize && pl) {
            for (i = 0; i < numberOfRows; i++)
                numberOfPoints += pl[i];
            grib_context_free(c, pl);
        }
        else {
            numberOfPoints = Ni * Nj;
        }
        bitmap  = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfPoints);
        pbitmap = bitmap;
        grib_get_long_array(gh, self->bitmap, bitmap, &numberOfPoints);
        for (i = 0; i < numberOfPoints; i++)
            n += *(bitmap++);

        grib_context_free(a->context, pbitmap);
    }
    else {
        if (plSize) {
            if (numberOfRows && !pl) return GRIB_INTERNAL_ERROR;
            for (i = 0; i < numberOfRows; i++)
                n += pl[i];
            grib_context_free(c, pl);
        }
        else {
            n = Ni * Nj;
        }
    }

    *count = n;
    return ret;
}


int grib_accessor_class_data_g1second_order_row_by_row_packing_t::unpack_float(grib_accessor* a, float* values, size_t* len){
    return unpack<float>(a, values, len);
}

int grib_accessor_class_data_g1second_order_row_by_row_packing_t::unpack_double(grib_accessor* a, double* values, size_t* len){
    return unpack<double>(a, values, len);
}

int grib_accessor_class_data_g1second_order_row_by_row_packing_t::pack_double(grib_accessor* a, const double* cval, size_t* len){
    int err         = 0;
    grib_handle* gh = grib_handle_of_accessor(a);
    char type[]     = "grid_second_order";
    size_t size     = strlen(type);

    err = grib_set_string(gh, "packingType", type, &size);
    if (err)
        return err;

    return grib_set_double_array(gh, "values", cval, *len);
}
