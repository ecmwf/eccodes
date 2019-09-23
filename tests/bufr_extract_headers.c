/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api_internal.h"
#include "eccodes.h"
#include <assert.h>

int main(int argc,char* argv[])
{
    char* filename;
    int i, err = 0;
    int num_messages = 0;
    codes_bufr_header* headers = NULL;
    codes_context* c = codes_context_get_default();

    assert(argc == 2);

    filename = argv[1];
    err = codes_bufr_extract_headers_malloc(c, filename, &headers, &num_messages);
    assert(!err);

    for (i=0; i<num_messages; ++i) {
        codes_bufr_header bh = headers[i];
        /*
         * Mimic the behaviour of bufr_get -f -p keys
         * for testing
         */
        const int has_ecmwf_local = (bh.localSectionPresent == 1 && bh.bufrHeaderCentre == 98);
        char rdbTypeStr[32] = "not_found";
        if (has_ecmwf_local) sprintf(rdbTypeStr, "%ld", bh.rdbType);
        printf("%ld %lu %ld %ld %ld %ld %s\n",
               bh.edition, bh.totalLength, bh.dataCategory,
               bh.masterTablesVersionNumber,
               bh.typicalMonth, bh.typicalDay,
               rdbTypeStr);
    }

    free(headers);

    return 0;
}
