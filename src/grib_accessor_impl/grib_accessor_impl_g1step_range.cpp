#include "grib_accessor_impl_g1step_range.h"

namespace eccodes {

    grib_accessor_impl_g1step_range::grib_accessor_impl_g1step_range(grib_section* p, grib_action* ga_creator) :
        grib_accessor_impl_abstract_long_vector(p, ga_creator)
    {
        // No extra logic here - init() handles this
    }

    grib_accessor_impl_g1step_range::~grib_accessor_impl_g1step_range() 
    {
        grib_context_free(context, v);
    }

    void grib_accessor_impl_g1step_range::init(const long len, grib_arguments* params)
    {
        // Default is to call parent's init, then init self (i.e. like a constructor)
        grib_accessor_impl_abstract_long_vector::init(len, params);
        init_g1step_range(len, params);
    }
    
    static const int u2s1[] = {
        60,                   /* (0) minutes */
        3600,                 /* (1) hour    */
        86400,                /* (2) day     */
        2592000,              /* (3) month     */
        -1,                   /* (4) */
        -1,                   /* (5) */
        -1,                   /* (6) */
        -1,                   /* (7) */
        -1,                   /* (8) */
        -1,                   /* (9) */
        10800,                /* (10) 3 hours */
        21600,                /* (11) 6 hours */
        43200,                /* (12) 12 hours */
        900,                  /* (13) 15 minutes  */
        1800,                 /* (14) 30 minutes */
        1 /* (15) seconds  */ /* See ECC-316 */
    };

    static const int units_index[] = {
        1, 0, 10, 11, 12, 2, 0, 13, 14, 15
    };

    static const int u2s[] = {
        60,      /* (0) minutes */
        3600,    /* (1) hour    */
        86400,   /* (2) day     */
        2592000, /* (3) month     */
        -1,      /* (4) */
        -1,      /* (5) */
        -1,      /* (6) */
        -1,      /* (7) */
        -1,      /* (8) */
        -1,      /* (9) */
        10800,   /* (10) 3 hours */
        21600,   /* (11) 6 hours */
        43200,   /* (12) 12 hours */
        1,       /* (13) seconds  */
        900,     /* (14) 15 minutes  */
        1800     /* (15) 30 minutes  */
    };

    void grib_accessor_impl_g1step_range::init_g1step_range(const long len, grib_arguments* params) 
    {
        grib_handle* h             = accessor_handle();
        int n                      = 0;
        p1                         = grib_arguments_get_name(h, params, n++);
        p2                         = grib_arguments_get_name(h, params, n++);
        timeRangeIndicator         = grib_arguments_get_name(h, params, n++);
        unit                       = grib_arguments_get_name(h, params, n++);
        step_unit                  = grib_arguments_get_name(h, params, n++);
        stepType                   = grib_arguments_get_name(h, params, n++);
        patch_fp_precip            = grib_arguments_get_name(h, params, n++);
        error_on_units             = 1;

        number_of_elements         = 2;
        v                          = (long*)grib_context_malloc_clear(h->context,
                                                sizeof(long) * number_of_elements);
        pack_index                 = -1;
        dirty                      = 1;
        length                     = 0;
    }

    size_t grib_accessor_impl_g1step_range::string_length() 
    { 
        return 255; 
    }

    int grib_accessor_impl_g1step_range::value_count(long* count) 
    { 
        *count = 1;
        return 0;
    }
    
    void grib_accessor_impl_g1step_range::dump(grib_dumper* dumper) 
    {
        grib_dump_string(dumper, as_accessor(), NULL);
    }

    int grib_accessor_impl_g1step_range::grib_g1_step_get_steps(long* start, long* theEnd)
    {
        int err = 0;
        long p1_val = 0, p2_val = 0, unit_val = 0, timeRangeIndicator_val = 0, timeRangeIndicatorFromStepRange = 0;
        long step_unit_val    = 1;
        char stepType_val[20] = {0,};
        size_t stepTypeLen = 20;
        long newstart, newend;
        int factor = 1;
        long u2sf, u2sf_step_unit;
        grib_handle* hand = accessor_handle();

        if (step_unit != NULL)
            grib_get_long_internal(hand, step_unit, &step_unit_val);

        if (err != GRIB_SUCCESS)
            return err;

        err = grib_get_long_internal(hand, unit, &unit_val);
        if (err)
            return err;
        if (unit_val == 254) {
            unit_val = 15; /* See ECC-316: WMO says 254 is for 'seconds' but we use 15! */
        }

        err = grib_get_long_internal(hand, p1, &p1_val);
        if (err)
            return err;

        err = grib_get_long_internal(hand, p2, &p2_val);
        if (err)
            return err;

        err = grib_get_long_internal(hand, timeRangeIndicator, &timeRangeIndicator_val);
        if (err)
            return err;

        /* TODO move to the def file */
        err = grib_get_long(hand, "timeRangeIndicatorFromStepRange", &timeRangeIndicatorFromStepRange);
        if (err) return err;

        if (timeRangeIndicatorFromStepRange == 10)
            timeRangeIndicator_val = timeRangeIndicatorFromStepRange;

        if (stepType) {
            err = grib_get_string_internal(hand, stepType, stepType_val, &stepTypeLen);
            if (err)
                return err;
        }
        else
            snprintf(stepType_val, sizeof(stepType_val), "unknown");

        *start  = p1_val;
        *theEnd = p2_val;

        if (timeRangeIndicator_val == 10)
            *start = *theEnd = (p1_val << 8) | (p2_val << 0);
        else if (!strcmp(stepType, "instant"))
            *start = *theEnd = p1_val;
        else if (!strcmp(stepType_val, "accum") && timeRangeIndicator == 0) {
            *start  = 0;
            *theEnd = p1_val;
        }

        if (u2s1[unit_val] == u2s[step_unit_val] || (*start == 0 && *theEnd == 0))
            return 0;

        newstart = (*start) * u2s1[unit_val];
        newend   = (*theEnd) * u2s1[unit_val];

        if (newstart < 0 || newend < 0) {
            factor = 60;
            u2sf   = u2s1[unit_val] / factor;
            if (u2s1[unit_val] % factor)
                return GRIB_DECODING_ERROR;
            newstart       = (*start) * u2sf;
            newend         = (*theEnd) * u2sf;
            u2sf_step_unit = u2s[step_unit_val] / factor;
            if (u2s[step_unit_val] % factor)
                return GRIB_DECODING_ERROR;
        }
        else {
            u2sf_step_unit = u2s[step_unit_val];
        }

        if (newstart % u2sf_step_unit != 0 || newend % u2sf_step_unit != 0) {
            return GRIB_DECODING_ERROR;
        }
        else {
            *start  = newstart / u2sf_step_unit;
            *theEnd = newend / u2sf_step_unit;
        }

        return 0;
    }

    int grib_accessor_impl_g1step_range::pack_long(pack_buffer& longs, std::size_t* packed_len) 
    {
        char buff[256];
        size_t bufflen                   = 100;
        char sval[100]                   = { 0 };
        char* p                          = sval;
        size_t svallen                   = 100;
        char stepType_val[20]                = {0,};
        size_t stepTypeLen = 20;
        long step_unit_val     = 0;
        int err            = 0;

        if (stepType) {
            err = grib_get_string_internal(accessor_handle(), stepType, stepType_val, &stepTypeLen);
            if (err)
                return err;
        }
        else
            snprintf(stepType_val, sizeof(stepType), "unknown");

        if (step_unit != NULL && (err = grib_get_long_internal(accessor_handle(), stepType, &step_unit_val)))
            return err;

        const long* val = longs.to_longs().ptr;

        switch (pack_index) {
            case -1:
                pack_index = -1;
                return pack_string(longs, packed_len);
            case 0:
            {
                pack_index     = -1;
                error_on_units = 0;
                unpack_string(char_view{sval, &svallen});
                error_on_units = 1;
                while (*p != '-' && *p != '\0')
                    p++;
                if (*p == '-') {
                    snprintf(buff, sizeof(buff), "%ld-%s", *val, ++p);
                }
                else {
                    if (strcmp(stepType, "instant") && strcmp(stepType, "avgd")) {
                        snprintf(buff, sizeof(buff), "%ld-%s", *val, sval);
                    }
                    else {
                        snprintf(buff, sizeof(buff), "%ld", *val);
                    }
                }
                pack_buffer in_buffer(context, const_char_view{buff, &bufflen});
                return pack_string(in_buffer, &bufflen);
            }
            case 1:
            {
                pack_index     = -1;
                error_on_units = 0;
                unpack_string(char_view{sval, &svallen});
                error_on_units = 1;
                while (*p != '-' && *p != '\0')
                    p++;
                if (*p == '-') {
                    *p = '\0';
                    snprintf(buff, sizeof(buff), "%s-%ld", sval, *val);
                }
                else {
                    if (strcmp(stepType, "instant") && strcmp(stepType, "avgd")) {
                        snprintf(buff, sizeof(buff), "%s-%ld", sval, *val);
                    }
                    else {
                        snprintf(buff, sizeof(buff), "%ld", *val);
                    }
                }
                pack_buffer in_buffer(context, const_char_view{buff, &bufflen});
                return pack_string(in_buffer, &bufflen);
            }
            default:
                Assert(pack_index < 2);
                break;
        }

        return GRIB_INTERNAL_ERROR;
    }
    
    static int grib_g1_step_apply_units(
            const long* start, const long* theEnd, const long* step_unit,
            long* P1, long* P2, long* unit,
            const int max, const int instant)
    {
        int j = 0;
        long start_sec, end_sec;
        int index     = 0;
        int max_index = sizeof(units_index) / sizeof(*units_index);

        while (*unit != units_index[index] && index != max_index)
            index++;

        start_sec = *start * u2s[*step_unit];
        *P2       = 0;

        if (instant) {
            *unit = units_index[0];
            for (j = index; j < max_index; j++) {
                if (start_sec % u2s1[*unit] == 0 &&
                    (*P1 = start_sec / u2s1[*unit]) <= max)
                    return 0;
                *unit = units_index[j];
            }
            for (j = 0; j < index; j++) {
                if (start_sec % u2s1[*unit] == 0 &&
                    (*P1 = start_sec / u2s1[*unit]) <= max)
                    return 0;
                *unit = units_index[j];
            }
        }
        else {
            end_sec = *theEnd * u2s[*step_unit];
            *unit   = units_index[0];
            for (j = index; j < max_index; j++) {
                if (start_sec % u2s1[*unit] == 0 &&
                    end_sec % u2s1[*unit] == 0 &&
                    (*P1 = start_sec / u2s1[*unit]) <= max &&
                    (*P2 = end_sec / u2s1[*unit]) <= max)
                    return 0;
                *unit = units_index[j];
            }
            for (j = 0; j < index; j++) {
                if (start_sec % u2s1[*unit] == 0 &&
                    end_sec % u2s1[*unit] == 0 &&
                    (*P1 = start_sec / u2s1[*unit]) <= max &&
                    (*P2 = end_sec / u2s1[*unit]) <= max)
                    return 0;
                *unit = units_index[j];
            }
        }

        return GRIB_WRONG_STEP;
    }

    int grib_accessor_impl_g1step_range::pack_string(pack_buffer& chars, std::size_t* packed_len)
    {
        grib_handle* h = accessor_handle();
        long timeRangeIndicator_val = 0, P1_val = 0, P2_val = 0;
        long start = 0, theEnd = -1, unit_val = 0, ounit = 0, step_unit_val = 1;
        int ret = 0;
        long end_sec, start_sec;
        char *p = NULL, *q = NULL;
        int instant       = 0;
        char stepType_val[20] = {0,};
        size_t stepTypeLen = 20;

        const char* val = chars.to_string().ptr;

        if (stepType) {
            ret = grib_get_string_internal(h, stepType, stepType_val, &stepTypeLen);
            if (ret)
                return ret;
        }
        else
            snprintf(stepType_val, sizeof(stepType_val), "unknown");

        if ((ret = grib_set_long_internal(h, "timeRangeIndicatorFromStepRange", -1)))
            return ret;

        /* don't change timeRangeIndicator when setting step EXCEPT IF instant*/
        if ((ret = grib_get_long_internal(h, timeRangeIndicator, &timeRangeIndicator_val)))
            return ret;

        instant = (strcmp(stepType_val, "instant") == 0) ? 1 : 0;

        if ((ret = grib_get_long_internal(h, unit, &unit_val)))
            return ret;
        if (unit_val == 254) {
            unit_val = 15; /* See ECC-316 */
        }

        if (step_unit != NULL && (ret = grib_get_long_internal(h, step_unit, &step_unit_val)))
            return ret;

        ounit = unit_val;

        start  = strtol(val, &p, 10);
        theEnd = start;
        if (*p != 0)
            theEnd = strtol(++p, &q, 10);

        if (start == 0 && theEnd == 0) {
            if ((ret = grib_set_long_internal(h, p1, start)) != GRIB_SUCCESS)
                return ret;
            ret = grib_set_long_internal(h, p2, theEnd);
            return ret;
        }
        end_sec   = theEnd * u2s[step_unit_val];
        start_sec = start * u2s[step_unit_val];

        if ((end_sec > 918000 || start_sec > 918000) &&
            h->context->gribex_mode_on && instant) {
            timeRangeIndicator_val = 10;
            if ((ret = grib_set_long_internal(h, timeRangeIndicator, 10)))
                return ret;
            /* TODO move to the def file*/
            if ((ret = grib_set_long_internal(h, "timeRangeIndicatorFromStepRange", 10)))
                return ret;
        }

        if (timeRangeIndicator_val == 10) {
            /*
            * timeRangeIndicator = 10 means 'P1 occupies octets 19 and 20' i.e. 16 bits
            */
            long off                   = 0;
            grib_accessor* p1_accessor = NULL;
            if (theEnd != start && !h->context->gribex_mode_on) {
                if (h->context->gribex_mode_on == 0) {
                    grib_context_log(h->context, GRIB_LOG_ERROR,
                                    "Unable to set %s: end must be equal to start when timeRangeIndicator=10",
                                    name);
                    return GRIB_WRONG_STEP;
                }
                else
                    start = theEnd;
            }
            if ((ret = grib_g1_step_apply_units(&start, &theEnd, &step_unit_val, &P1_val, &P2_val, &unit_val, 65535, instant)) != GRIB_SUCCESS) {
                grib_context_log(h->context, GRIB_LOG_ERROR, "unable to find units to set %s=%s", name, val);
                return ret;
            }

            p1_accessor = grib_find_accessor(accessor_handle(), p1);
            if (p1_accessor == NULL) {
                grib_context_log(h->context, GRIB_LOG_ERROR, "unable to find accessor %s", p1);
                return GRIB_NOT_FOUND;
            }
            off = p1_accessor->offset * 8;
            /* Note: here we assume the key P2 is one octet and immediately follows P1. Hence 16 bits */

            ret = grib_encode_unsigned_long(accessor_handle()->buffer->data, P1_val, &off, 16);
            if (ret != 0)
                return ret;

            if (h->context->debug) {
                long dp1,dp2;
                grib_get_long(h, p1, &dp1);
                grib_get_long(h, p2, &dp2);
                fprintf(stderr, "ECCODES DEBUG pack_string: P1=%ld P2=%ld (as two octets => %ld)\n", dp1, dp2, P1_val);
            }

            if (ounit != unit_val)
                ret = grib_set_long_internal(h, unit, unit_val);

            return ret;
        }

        if ((ret = grib_g1_step_apply_units(&start, &theEnd, &step_unit_val, &P1_val, &P2_val, &unit_val, 255, instant)) != GRIB_SUCCESS) {
            if (instant || h->context->gribex_mode_on) {
                long off                   = 0;
                grib_accessor* p1_accessor = NULL;
                if ((ret = grib_set_long_internal(h, timeRangeIndicator, 10)))
                    return ret;
                /* TODO move to the def file*/
                if ((ret = grib_set_long_internal(h, "timeRangeIndicatorFromStepRange", 10)))
                    return ret;
                if (theEnd != start && !h->context->gribex_mode_on) {
                    grib_context_log(h->context, GRIB_LOG_ERROR,
                                    "Unable to set %s: end must be equal to start when timeRangeIndicator=10",
                                    name);
                    return GRIB_WRONG_STEP;
                }
                else
                    start = theEnd;

                if ((ret = grib_g1_step_apply_units(&start, &theEnd, &step_unit_val, &P1_val, &P2_val, &unit_val, 65535, instant)) != GRIB_SUCCESS) {
                    grib_context_log(h->context, GRIB_LOG_ERROR, "unable to find units to set %s=%s", name, val);
                    return ret;
                }

                p1_accessor = grib_find_accessor(accessor_handle(), p1);
                if (p1_accessor == NULL) {
                    grib_context_log(h->context, GRIB_LOG_ERROR, "unable to find accessor %s", p1);
                    return GRIB_NOT_FOUND;
                }
                off = p1_accessor->offset * 8;
                /* Note:  case for timeRangeIndicator of 10
                * We assume the key P2 is one octet and immediately follows P1. Hence 16 bits
                */
                ret = grib_encode_unsigned_long(accessor_handle()->buffer->data, P1_val, &off, 16);
                if (ret != 0)
                    return ret;

                if (h->context->debug) {
                    long dp1,dp2;
                    grib_get_long(h, p1, &dp1);
                    grib_get_long(h, p2, &dp2);
                    fprintf(stderr, "ECCODES DEBUG pack_string: P1=%ld P2=%ld (as two octets => %ld)\n", dp1, dp2, P1_val);
                }

                if (ounit != unit_val)
                    ret = grib_set_long_internal(h, unit, unit_val);
            }

            if (ret == GRIB_WRONG_STEP) {
                grib_context_log(h->context, GRIB_LOG_ERROR,
                        "Failed to set %s=%s: Keys P1 and P2 are one octet each (Range 0 to 255)", name, val);
            }
            return ret;
        }

        if (ounit != unit_val)
            if ((ret = grib_set_long_internal(h, unit, unit_val)) != GRIB_SUCCESS)
                return ret;

        if ((ret = grib_set_long_internal(h, p1, P1_val)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_set_long_internal(h, p2, P2_val)) != GRIB_SUCCESS)
            return ret;

        v[0] = start;
        v[1] = theEnd;
        dirty   = 0;

        return 0;
    }

    int grib_accessor_impl_g1step_range::unpack_long(long_view longs)
    {
        char buff[100];
        size_t bufflen = 100;
        long start, theEnd;
        char* p = buff;
        char* q = NULL;
        int err = 0;

        /*TODO implement dirty*/
        if ((err = unpack_string(char_view{buff, &bufflen})) != GRIB_SUCCESS)
            return err;

        start  = strtol(buff, &p, 10);
        theEnd = start;
        if (*p != 0)
            theEnd = strtol(++p, &q, 10);

        if (pack_index == 1)
            *longs.ptr = start;
        else
            *longs.ptr = theEnd;

        v[0] = start;
        v[1] = theEnd;
        dirty   = 0;

        return 0;
    }

    int grib_accessor_impl_g1step_range::unpack_string(char_view chars)
    {
        char buf[100];
        size_t size = 0;
        long start = 0, theEnd = 0;
        long timeRangeIndicator_val = 0;
        long unit_val;
        int err            = 0;
        char stepType_val[20]  = {0,};
        size_t stepTypeLen = 20;
        grib_handle* hand  = accessor_handle();

        if ((err = grib_g1_step_get_steps(&start, &theEnd)) != GRIB_SUCCESS) {
            size_t step_unit_string_len = 10;
            char step_unit_string[10];

            if (step_unit != NULL)
                grib_get_string(hand, step_unit, step_unit_string, &step_unit_string_len);
            else
                snprintf(step_unit_string, sizeof(step_unit_string), "h");

            if (error_on_units) {
                grib_get_long_internal(hand, unit, &unit_val);
                if (unit_val == 254) {
                    unit_val = 15; /* See ECC-316 */
                }
                grib_set_long_internal(hand, step_unit, unit_val);
                grib_context_log(context, GRIB_LOG_ERROR,
                                "unable to represent the step in %s\n                    Hint: try changing the step units",
                                step_unit_string);
            }
            return err;
        }

        err = grib_get_long_internal(hand, timeRangeIndicator, &timeRangeIndicator_val);
        if (err)
            return err;

        if (stepType) {
            err = grib_get_string_internal(hand, stepType, stepType_val, &stepTypeLen);
            if (err)
                return err;
        }
        else
            snprintf(stepType_val, sizeof(stepType_val), "unknown");

        /* Patch for old forecast probabilities */
        if (patch_fp_precip) {
            start += 24;
        }

        if (strcmp(stepType, "instant") == 0) {
            snprintf(buf, sizeof(buf), "%ld", start);
        }
        else if ((strcmp(stepType, "avgfc") == 0) ||
                (strcmp(stepType, "avgua") == 0) ||
                (strcmp(stepType, "avgia") == 0) ||
                (strcmp(stepType, "varins") == 0)) {
            snprintf(buf, sizeof(buf), "%ld", start);
        }
        else if (
            (strcmp(stepType, "accum") == 0) ||
            (strcmp(stepType, "avg") == 0) ||
            (strcmp(stepType, "min") == 0) ||
            (strcmp(stepType, "max") == 0) ||
            (strcmp(stepType, "rms") == 0) ||
            (strcmp(stepType, "diff") == 0) ||
            (strcmp(stepType, "avgas") == 0) ||
            (strcmp(stepType, "avgad") == 0) ||
            (strcmp(stepType, "avgid") == 0) ||
            (strcmp(stepType, "varas") == 0) ||
            (strcmp(stepType, "varad") == 0)) {
            if (start == theEnd) {
                snprintf(buf, sizeof(buf), "%ld", theEnd);
            }
            else {
                snprintf(buf, sizeof(buf), "%ld-%ld", start, theEnd);
            }
        }
        else {
            grib_context_log(context, GRIB_LOG_ERROR, "Unknown stepType=[%s] timeRangeIndicator=[%ld]", stepType, timeRangeIndicator);
            return GRIB_NOT_IMPLEMENTED;
        }

        size = strlen(buf) + 1;

        if (*chars.len < size)
            return GRIB_ARRAY_TOO_SMALL;

        *chars.len = size;

        memcpy(chars.ptr, buf, size);

        return GRIB_SUCCESS;
    }

}
