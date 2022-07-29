#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "grib_api_internal.h"

int main(int argc, char* argv[])
{
    FILE* f         = NULL;
    grib_handle* h = NULL;
    int err       = 0;

    Assert(argc == 2);
    f = fopen(argv[1], "rb");
    Assert(f);

    while ((h = grib_handle_new_from_file(0, f, &err)) != NULL) {
        grib_keys_iterator* kiter = NULL;

        /* Use namespace of NULL to get ALL keys */
        /* Set flags to 0 to not filter any keys */
        kiter = grib_keys_iterator_new(h, /*flags=*/0, /*namespace=*/NULL);
        Assert(kiter);

        while (grib_keys_iterator_next(kiter)) {
            const char* name = grib_keys_iterator_get_name(kiter);
            Assert(name);
            printf("%s\n", name);
        }

        grib_keys_iterator_delete(kiter);
        grib_handle_delete(h);
    }
    fclose(f);
    return 0;
}
