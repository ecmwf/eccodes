#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "eccodes.h"

int main(int argc, char* argv[])
{
    FILE* f         = NULL;
    codes_handle* h = NULL;
    int err       = 0;

    assert(argc == 2);
    f = fopen(argv[1], "rb");
    assert(f);

    while ((h = codes_handle_new_from_file(0, f, PRODUCT_GRIB, &err)) != NULL) {
        codes_keys_iterator* kiter = NULL;

        /* Use namespace of NULL to get ALL keys */
        /* Set flags to 0 to not filter any keys */
        kiter = codes_keys_iterator_new(h, /*flags=*/0, /*namespace=*/NULL);
        assert(kiter);

        while (codes_keys_iterator_next(kiter)) {
            const char* name = codes_keys_iterator_get_name(kiter);
            assert(name);
            printf("%s\n", name);
        }

        codes_keys_iterator_delete(kiter);
        codes_handle_delete(h);
    }
    fclose(f);
    return 0;
}
