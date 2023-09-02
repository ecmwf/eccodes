
static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_g2end_step* self = (grib_accessor_g2end_step*)a;
    int err = 0;
    long start_step;
    long unit;
    long coded_unit;
    long coded_time_range, typeOfTimeIncrement, numberOfTimeRange;
    long coded_time_range_sec=0;
    int factor;
    long u2sf,u2sf_step_unit;
    int add_time_range = 1; /* whether we add lengthOfTimeRange */

    grib_handle* h=grib_handle_of_accessor(a);

    if((err = grib_get_long_internal(h,self->start_step,&start_step))) return err;

    /*point in time */
    if (self->year == NULL) {
        *val=start_step;
        return 0;
    }

    if((err = grib_get_long_internal(h,self->unit,&unit))) return err;

    if((err = grib_get_long_internal(h,self->coded_unit,&coded_unit))) return err;
    if((err = grib_get_long_internal(h,self->coded_time_range, &coded_time_range))) return err;
    if((err = grib_get_long_internal(h,self->typeOfTimeIncrement, &typeOfTimeIncrement))) return err;
    if((err = grib_get_long_internal(h,self->numberOfTimeRange, &numberOfTimeRange))) return err;

    Assert(numberOfTimeRange == 1 || numberOfTimeRange == 2);

    err = convert_time_range(h, unit, coded_unit, &coded_time_range);
    if (err != GRIB_SUCCESS) return err;

#if 0
    if (coded_unit!=unit) {
        coded_time_range_sec=coded_time_range*u2s2[coded_unit];
        if (coded_time_range_sec<0) {
            factor=60;
            if (u2s2[coded_unit] % factor) return GRIB_DECODING_ERROR;
            if (u2s[unit] % factor) return GRIB_DECODING_ERROR;
            u2sf=u2s2[coded_unit]/factor;
            coded_time_range_sec=coded_time_range*u2sf;
            u2sf_step_unit=u2s[unit]/factor;
        } else {
            u2sf_step_unit=u2s[unit];
        }
        if (coded_time_range_sec % u2sf_step_unit!=0) {
            grib_context_log(h->context,GRIB_LOG_ERROR,"unable to convert endStep in stepUnits");
            return GRIB_WRONG_STEP_UNIT;
        }
        coded_time_range = coded_time_range_sec / u2sf_step_unit;
    }
#endif
    if (typeOfTimeIncrement == 1) {
        /* See GRIB-488 */
        /* Note: For this case, lengthOfTimeRange is not related to step and should not be used to calculate step */
        add_time_range = 0;
        if (is_special_expver(h)) {
            add_time_range = 1;
        }
    }
    if (add_time_range) {
        *val = start_step + coded_time_range;
    } else {
        *val = start_step;
    }

    return GRIB_SUCCESS;
}

