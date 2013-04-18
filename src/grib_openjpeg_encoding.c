/*
 * Copyright 2005-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#if HAVE_LIBOPENJPEG
#include "openjpeg.h"

/* TODO: Strip the \n from msg */
static void openjpeg_warning(const char *msg, void *client_data)
{
	grib_context_log((grib_context*)client_data,GRIB_LOG_WARNING,"OPENJPEG: %s",msg);
}

static void openjpeg_error(const char *msg, void *client_data)
{
	grib_context_log((grib_context*)client_data,GRIB_LOG_ERROR,"OPENJPEG: %s",msg);
}

static void openjpeg_info(const char *msg, void *client_data)
{
	/* grib_context_log((grib_context*)client_data,GRIB_LOG_INFO,"openjpeg: %s",msg); */
}

int grib_openjpeg_encode(grib_context *c, j2k_encode_helper *helper) {

	int err = GRIB_SUCCESS;
	const int numcomps = 1;

	int i;

	const double * values   = helper->values;
	long  no_values        = helper->no_values;
	double reference_value = helper->reference_value;
	double divisor         = helper->divisor;
	double decimal         = helper->decimal;
	int*   data;

	opj_cparameters_t parameters = {0,};	/* compression parameters */
	opj_event_mgr_t event_mgr = {0,};		/* event manager */
	opj_image_t *image = NULL;
	opj_image_cmptparm_t cmptparm = {0,};
	opj_cio_t *cio = NULL;
	opj_cinfo_t* cinfo = NULL;


	/* set encoding parameters to default values */
	opj_set_default_encoder_parameters(&parameters);

	parameters.tcp_numlayers  = 1;
	parameters.cp_disto_alloc = 1;
	parameters.tcp_rates[0]   = helper->compression;

	/* initialize image component */  
	cmptparm.prec = helper->bits_per_value; 
	cmptparm.bpp  = helper->bits_per_value; /* Not sure about this one and the previous. What is the difference? */
	cmptparm.sgnd = 0;
	cmptparm.dx   = 1;
	cmptparm.dy   = 1;
	cmptparm.w    = helper->width;
	cmptparm.h    = helper->height;

	/* create the image */
	image = opj_image_create(numcomps, &cmptparm, CLRSPC_GRAY);

	if(!image) {
		err = GRIB_ENCODING_ERROR;
		goto cleanup;
	}

	image->x0 = 0;
	image->y0 = 0;
	image->x1 = helper->width;
	image->y1 = helper->height;

	Assert(cmptparm.prec <= sizeof(image->comps[0].data[0])*8 - 1); /* BR: -1 because I don't know what happens if the sign bit is set */

	Assert(helper->no_values ==  image->comps[0].h * image->comps[0].w);

	/* Simple packing */
	data = image->comps[0].data;
	for(i=0;i< no_values;i++){
		unsigned long unsigned_val = (unsigned long)((((values[i]*decimal)-(reference_value))*divisor)+0.5);
		data[i] = unsigned_val;
	}

	/* get a J2K compressor handle */
	cinfo = opj_create_compress(CODEC_J2K);

	/* catch events using our callbacks and give a local context */
	event_mgr.error_handler   = openjpeg_error;
	event_mgr.info_handler    = openjpeg_info;
	event_mgr.warning_handler = openjpeg_warning;
	opj_set_event_mgr((opj_common_ptr)cinfo, &event_mgr, c);

	/* setup the encoder parameters using the current image and user parameters */
	opj_setup_encoder(cinfo, &parameters, image);

	/* open a byte stream for writing */
	/* allocate memory for all tiles */
	cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0);

	/* encode image */
	if (!opj_encode(cinfo, cio, image, NULL)) {
		err = GRIB_ENCODING_ERROR;
		goto cleanup;
	}

	helper->jpeg_length = cio_tell(cio);
	memcpy(helper->jpeg_buffer,cio->buffer,helper->jpeg_length);

cleanup:
	if (cio)   opj_cio_close(cio);
	if (cinfo) opj_destroy_compress(cinfo);
	if (image) opj_image_destroy(image);

	return err;
}

int grib_openjpeg_decode(grib_context *c, unsigned char *buf, size_t *buflen, double *val, size_t *n_vals) {

	int err = GRIB_SUCCESS;
	int i;
	unsigned long  mask;
	int *data;
	size_t count;

	opj_dparameters_t parameters = {0,};	/* decompression parameters */
	opj_dinfo_t* dinfo = NULL;	/* handle to a decompressor */
	opj_event_mgr_t event_mgr = {0,};		/* event manager */
	opj_cio_t *cio = NULL;
	opj_image_t *image = NULL;
	opj_image_comp_t comp = {0,};


	/* set decoding parameters to default values */
	opj_set_default_decoder_parameters(&parameters);

	/* JPEG-2000 codestream */

	/* get a decoder handle */
	dinfo = opj_create_decompress(CODEC_J2K);

	/* catch events using our callbacks and give a local context */
	event_mgr.error_handler   = openjpeg_error;
	event_mgr.info_handler    = openjpeg_info;
	event_mgr.warning_handler = openjpeg_warning;

	opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, c);

	/* setup the decoder decoding parameters using user parameters */
	opj_setup_decoder(dinfo, &parameters);

	/* open a byte stream */
	cio = opj_cio_open((opj_common_ptr)dinfo, buf, *buflen);

	image = opj_decode(dinfo, cio);

	if(!image) {
		grib_context_log(c, GRIB_LOG_ERROR, "OPENJPEG: failed to decode image");
		err = GRIB_DECODING_ERROR;
		goto cleanup;
	}

	if ( !(*n_vals <= image->comps[0].w * image->comps[0].h) ) {
		err = GRIB_DECODING_ERROR;
		goto cleanup;
	}

	if ( (image->numcomps != 1) || !(image->x1 * image->y1) ) {
		err = GRIB_DECODING_ERROR;
		goto cleanup;
	}

	Assert(image->comps[0].sgnd == 0);
	Assert(comp.prec <= sizeof(image->comps[0].data[0])*8 - 1); /* BR: -1 because I don't know what happens if the sign bit is set */

	Assert(image->comps[0].prec < sizeof(mask)*8-1);

	data = image->comps[0].data;
	mask = (1 << image->comps[0].prec) - 1;

	count = image->comps[0].w * image->comps[0].h;

	for(i = 0; i <count ; i++)
			val[i] = data[i] & mask;

cleanup:
	/* close the byte stream */
	if (cio)   opj_cio_close(cio);
	if (dinfo) opj_destroy_decompress(dinfo);
	if (image) opj_image_destroy(image);

	return err;
}

#else

int grib_openjpeg_decode(grib_context *c, unsigned char *buf, size_t *buflen, double *val, size_t *n_vals) {
	grib_context_log(c, GRIB_LOG_ERROR,
			"grib_accessor_data_jpeg2000_packing : openjpeg jpeg support not enabled. Please rerun configure with --with-openjpeg-support");
	return GRIB_NOT_IMPLEMENTED;
}

int grib_openjpeg_encode(grib_context *c, j2k_encode_helper *helper) {
	grib_context_log(c, GRIB_LOG_ERROR,
			"grib_accessor_data_jpeg2000_packing : openjpeg jpeg support not enabled. Please rerun configure with --with-openjpeg-support");
	return GRIB_NOT_IMPLEMENTED;
}

#endif

