/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "eccodes.h"

void usage(char* prog) {
    printf("usage: %s in.nc\n",prog);
    exit(1);
}

int main(int argc,char* argv[]) {

    char* file;
    int err=0;
    codes_handle* h;
    char identifier[7]={0,};
    size_t len=7;
    codes_context* c=codes_context_get_default();

    if (argc>2) usage(argv[0]);

    file=argv[1];

    h=codes_handle_new_from_nc_file(c,file,&err);
    codes_get_string(h,"identifier",identifier,&len);
    printf("%s\n",identifier);
    CODES_CHECK(err,0);

    return err;
}
