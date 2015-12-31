/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: mars_param
 *
 * Description: how to query MARS params
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "eccodes.h"

int main(int argc, char** argv)
{
    codes_string_list* list;
    codes_string_list* list2;

    list=codes_grib_util_get_param_id("11.2");
    printf("mars.param=11.2 -> paramId= ");
    while (list) {
        printf("%s ",list->value);
        list2=codes_grib_util_get_param_id("130.128");
        printf("mars.param=11.2 -> paramId= ");
        while (list2) {
            printf("%s ",list2->value);
            list2=list2->next;
        }
        list=list->next;
    }
    printf("\n");
    printf("\n");

    printf("paramId=130 -> mars.param= ");
    list=codes_grib_util_get_mars_param("130");
    while (list) {
        printf("%s ",list->value);
        list=list->next;
    }
    printf("\n");

    return 0;
}
