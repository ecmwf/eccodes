/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#if HAVE_LIBJASPER
/* Remove compiler warnings re macros being redefined */
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "jasper/jasper.h"

#define MAXOPTSSIZE 1024

int grib_jasper_decode(grib_context *c,unsigned char *buf, size_t *buflen, double *values, size_t *no_values)
{
    /*jas_setdbglevel(99999);*/
    jas_image_t  *image = NULL;
    jas_stream_t *jpeg  = NULL;
    int code = GRIB_SUCCESS;
    jas_matrix_t *matrix = NULL;
    jas_image_cmpt_t *p;
    int i,j,k;

    jpeg = jas_stream_memopen((char*)buf,*buflen);
    if(!jpeg) {
        code = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    grib_context_log(c, GRIB_LOG_DEBUG, "grib_jasper_decode: Jasper version %s", jas_getversion());

    image = jpc_decode(jpeg,NULL);
    if(!image) {
        code = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    p = image->cmpts_[0];

    if (image->numcmpts_ != 1 ) {
        /* Image not gray scale */
        code = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    matrix = jas_matrix_create(jas_image_height(image), jas_image_width(image));

    if(!matrix) {
        code = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    jas_image_readcmpt(image,0,0,0,jas_image_width(image), jas_image_height(image),matrix);

    Assert(p->height_ * p->width_ == *no_values);

    k=0;
    for (i=0;i<p->height_;i++)
        for (j=0;j<p->width_;j++) {
            values[k++] = matrix->rows_[i][j];
        }

cleanup:
    if(matrix) jas_matrix_destroy(matrix);
    if(image)  jas_image_destroy(image);
    if(jpeg)   jas_stream_close(jpeg);

    return code;
}

int grib_jasper_encode(grib_context *c, j2k_encode_helper *helper)
{
    int code = GRIB_SUCCESS;
    int jaserr;

    char opts[MAXOPTSSIZE];
    double reference_value = helper->reference_value;
    double decimal = helper->decimal;
    double divisor = helper->divisor;
    const double* values = helper->values;
    long no_values = helper->no_values;
    long bits8;
    int i;

    size_t buflen = 0;
    unsigned char *encoded = NULL;
    unsigned char *p = NULL;

    jas_image_t image = {0};
    jas_stream_t *jpcstream = 0;
    jas_stream_t *istream = 0;
    jas_image_cmpt_t cmpt = {0,};
    jas_image_cmpt_t *pcmpt = 0;

    image.tlx_      = 0;
    image.tly_      = 0;
    image.brx_      = helper->width;
    image.bry_      = helper->height;
    image.numcmpts_ = 1;
    image.maxcmpts_ = 1;
    image.clrspc_   = JAS_CLRSPC_SGRAY;
    image.cmprof_   = 0;

#if JASPER_VERSION_MAJOR == 1
    /* ECC-396: Support for Jasper 2.0
     * Jasper version 1 had the 'inmem_' data member but
     * version 2 removed it from the interface */
    image.inmem_    = 1;
#endif

    cmpt.tlx_       = 0;
    cmpt.tly_       = 0;
    cmpt.hstep_     = 1;
    cmpt.vstep_     = 1;
    cmpt.width_     = helper->width;
    cmpt.height_    = helper->height;
    cmpt.type_      = JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y);
    cmpt.prec_      = helper->bits_per_value;
    cmpt.sgnd_      = 0;
    cmpt.cps_       = (helper->bits_per_value+7)/8;

    /* Simple packing encoding */
    bits8   = (helper->bits_per_value+7)/8*8;
    Assert(bits8 > 0);
    encoded = (unsigned char*)grib_context_malloc_clear(c,bits8/8*no_values);

    if(!encoded) {
        code = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }

    buflen = 0;
    p      = encoded;

    for(i=0;i< no_values;i++){
        long blen = bits8;
        unsigned long unsigned_val = (unsigned long)((((values[i]*decimal)-(reference_value))*divisor)+0.5);
        while(blen >= 8)
        {
            blen   -= 8;
            *p = (unsigned_val >> blen);
            p++;
            buflen++;
        }
    }

    /*jas_init();*/

    opts[0] = 0;

    if( helper->compression != 0)
    {
        /* Lossy */
#ifndef ECCODES_ON_WINDOWS
        snprintf (opts, MAXOPTSSIZE, "mode=real\nrate=%f", 1.0/helper->compression);
#else
        /* Microsoft Windows Visual Studio support */
        _snprintf(opts, MAXOPTSSIZE, "mode=real\nrate=%f", 1.0/helper->compression);
#endif
    }

    Assert(cmpt.width_ * cmpt.height_ * cmpt.cps_ == buflen);
    grib_context_log(c, GRIB_LOG_DEBUG, "grib_jasper_encode: Jasper version %s", jas_getversion());

    pcmpt           = &cmpt;
    image.cmpts_    = &pcmpt;

    istream         = jas_stream_memopen((char *)encoded,buflen);
    cmpt.stream_    = istream;

    jpcstream       = jas_stream_memopen((char*)helper->jpeg_buffer,helper->buffer_size);
    jaserr          = jpc_encode(&image,jpcstream,opts);

    if(jaserr != 0) {
        /* increase the number of guard bits */
        strcat(opts,"\nnumgbits=4");
        grib_context_log(c, GRIB_LOG_ERROR, "JASPER: error %d, increasing the number of guard bits", jaserr);
        jas_stream_close(istream);   istream = 0;
        jas_stream_close(jpcstream); jpcstream = 0;

        istream         = jas_stream_memopen((char *)encoded,buflen);
        cmpt.stream_    = istream;
        jpcstream       = jas_stream_memopen((char*)helper->jpeg_buffer,helper->buffer_size);
        jaserr          = jpc_encode(&image,jpcstream,opts);
    }

    if ( jaserr != 0 ) {
        grib_context_log(c, GRIB_LOG_ERROR, "JASPER: error %d", jaserr);
        code = GRIB_ENCODING_ERROR;
        goto cleanup;
    }

    helper->jpeg_length = jpcstream->rwcnt_;
    jaserr=jas_stream_close(istream); istream = 0;
    jaserr=jas_stream_close(jpcstream);jpcstream = 0;

cleanup:
    grib_context_free(c,encoded);
    if(istream)   jas_stream_close(istream);
    if(jpcstream) jas_stream_close(jpcstream);

    return code;
}

#else

int grib_jasper_decode(grib_context *c, unsigned char *buf, size_t *buflen, double *val, size_t *n_vals)
{
    grib_context_log(c, GRIB_LOG_ERROR,
            "grib_accessor_data_jpeg2000_packing: Jasper JPEG support not enabled.");
    return GRIB_NOT_IMPLEMENTED;
}

int grib_jasper_encode(grib_context *c, j2k_encode_helper *helper)
{
    grib_context_log(c, GRIB_LOG_ERROR,
            "grib_accessor_data_jpeg2000_packing: Jasper JPEG support not enabled.");
    return GRIB_NOT_IMPLEMENTED;
}

#endif
