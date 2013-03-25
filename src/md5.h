
#ifndef md5_H
#define md5_H

#include <stdlib.h>
#include <stdint.h>


typedef struct grib_md5_state {
    uint64_t size;

    unsigned long words[64];
    unsigned long word_count;

    unsigned char bytes[4];
    unsigned long byte_count;

    unsigned long h0;
    unsigned long h1;
    unsigned long h2;
    unsigned long h3;

} grib_md5_state;

void grib_md5_init(grib_md5_state* s);
void grib_md5_add(grib_md5_state* s,const void* data,size_t len);
void grib_md5_end(grib_md5_state* s, char *digest);



#endif
