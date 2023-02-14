/* ECC-1269 */
/* Old logic moved from src/grib_util.c function:grib_util_set_spec2  */
 
    if (flags & GRIB_UTIL_SET_SPEC_FLAGS_ONLY_PACKING) {
        if (packing_spec->packing == GRIB_UTIL_PACKING_USE_PROVIDED &&
            strcmp(input_packing_type, "grid_simple_matrix")) {
            switch (packing_spec->packing_type) {
                case GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX:
                    if (strcmp(input_packing_type, "spectral_complex") && !strcmp(input_packing_type, "spectral_simple"))
                        SET_STRING_VALUE("packingType", "spectral_complex");
                    break;
                case GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE:
                    if (strcmp(input_packing_type, "spectral_simple") && !strcmp(input_packing_type, "spectral_complex"))
                        SET_STRING_VALUE("packingType", "spectral_simple");
                    break;
                case GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE:
                    if (strcmp(input_packing_type, "grid_simple") && !strcmp(input_packing_type, "grid_complex"))
                        SET_STRING_VALUE("packingType", "grid_simple");
                    break;
                case GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE_MATRIX:
                    SET_STRING_VALUE("packingType", "grid_simple_matrix");
                    break;
                case GRIB_UTIL_PACKING_TYPE_GRID_COMPLEX:
                    if (strcmp(input_packing_type, "grid_complex") && !strcmp(input_packing_type, "grid_simple"))
                        SET_STRING_VALUE("packingType", "grid_complex");
                    break;
                case GRIB_UTIL_PACKING_TYPE_JPEG:
                    if (strcmp(input_packing_type, "grid_jpeg") && !strcmp(input_packing_type, "grid_simple"))
                        SET_STRING_VALUE("packingType", "grid_jpeg");
                    break;
                case GRIB_UTIL_PACKING_TYPE_CCSDS:
                    if (strcmp(input_packing_type, "grid_ccsds") && !strcmp(input_packing_type, "grid_simple"))
                        SET_STRING_VALUE("packingType", "grid_ccsds");
                    break;
                case GRIB_UTIL_PACKING_TYPE_IEEE:
                    if (strcmp(input_packing_type, "grid_ieee") && !strcmp(input_packing_type, "grid_simple"))
                        SET_STRING_VALUE("packingType", "grid_ieee");
                    break;
                case GRIB_UTIL_PACKING_TYPE_GRID_SECOND_ORDER:
                    /* we delay the set of grid_second_order because we don't want
                       to do it on a field with bitsPerValue=0 */
                    setSecondOrder = 1;
                    break;
                default:
                    fprintf(stderr, "GRIB_UTIL_SET_SPEC: invalid packing_spec->packing_type (%ld)\n",
                            (long)packing_spec->packing_type);
                    *err = GRIB_INTERNAL_ERROR;
                    goto cleanup;
                    break;
            }
        }
        switch (packing_spec->accuracy) {
            case GRIB_UTIL_ACCURACY_SAME_BITS_PER_VALUES_AS_INPUT:
                break;

            case GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES:
                if (input_bits_per_value != packing_spec->bitsPerValue)
                    SET_LONG_VALUE("bitsPerValue", packing_spec->bitsPerValue);
                break;

            case GRIB_UTIL_ACCURACY_SAME_DECIMAL_SCALE_FACTOR_AS_INPUT:
                break;

            case GRIB_UTIL_ACCURACY_USE_PROVIDED_DECIMAL_SCALE_FACTOR:
                if (input_decimal_scale_factor != packing_spec->decimalScaleFactor)
                    SET_LONG_VALUE("decimalScaleFactor", packing_spec->decimalScaleFactor);
                break;

            default:
                fprintf(stderr, "GRIB_UTIL_SET_SPEC: invalid packing_spec->accuracy (%ld)\n", (long)packing_spec->accuracy);
                *err = GRIB_INTERNAL_ERROR;
                goto cleanup;
                break;
        }

        /*nothing to be changed*/
        if (count == 0) {
            *err = -1;
            return h;
        }

        if (h->context->debug == -1) {
            print_values(h->context, spec, packing_spec, data_values, data_values_count, values, count);
        }

        if ((*err = grib_set_values(h, values, count)) != 0) {
            fprintf(stderr, "GRIB_UTIL_SET_SPEC: Cannot set values: %s\n", grib_get_error_message(*err));
            for (i = 0; i < count; i++)
                if (values[i].error) fprintf(stderr, " %s %s\n", values[i].name, grib_get_error_message(values[i].error));
            goto cleanup;
        }
        if (h->context->debug == -1) {
            int j = 0;
            fprintf(stderr, "ECCODES DEBUG grib_util: grib_set_double_array\n");
            for (j = 0; j < 20; j++)
                fprintf(stderr, "ECCODES DEBUG grib_util %g\n", data_values[j]);
            fprintf(stderr, "ECCODES DEBUG grib_util: data_values_count=%d \n", (int)data_values_count);
        }

        if ((*err = grib_set_double_array(h, "values", data_values, data_values_count)) != 0) {
            goto cleanup;
        }
        if (h->context->debug == -1)
            fprintf(stderr, "ECCODES DEBUG grib_util: done grib_set_double_array \n");

        /* convert to second_order if not constant field */
        if (setSecondOrder) {
            int constant        = 0;
            double missingValue = 0;
            grib_get_double(h, "missingValue", &missingValue);
            constant = is_constant_field(missingValue, data_values, data_values_count);

            if (!constant) {
                size_t packTypeLen;
                if (editionNumber == 1) {
                    long numberOfGroups;
                    grib_handle* htmp = grib_handle_clone(h);

                    packTypeLen = 17;
                    grib_set_string(htmp, "packingType", "grid_second_order", &packTypeLen);
                    grib_get_long(htmp, "numberOfGroups", &numberOfGroups);
                    /* GRIBEX is not able to decode overflown numberOfGroups with SPD */
                    if (numberOfGroups > 65534 && h->context->no_spd) {
                        packTypeLen = 24;
                        grib_set_string(h, "packingType", "grid_second_order_no_SPD", &packTypeLen);
                        grib_handle_delete(htmp);
                    }
                    else {
                        grib_handle_delete(h);
                        h = htmp;
                    }
                }
                else {
                    packTypeLen = 17;
                    grib_set_string(h, "packingType", "grid_second_order", &packTypeLen);
                    grib_set_double_array(h, "values", data_values, data_values_count);
                }
            }
            else {
                if (h->context->gribex_mode_on) {
                    h->context->gribex_mode_on = 0;
                    grib_set_double_array(h, "values", data_values, data_values_count);
                    h->context->gribex_mode_on = 1;
                }
            }
        }

        return h;
    } /* flags & GRIB_UTIL_SET_SPEC_FLAGS_ONLY_PACKING */

