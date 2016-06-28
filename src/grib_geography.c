/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

#include <math.h>

#define NUMBER(x) (sizeof(x)/sizeof(x[0]))
#define MAXITER  10

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static void gauss_first_guess(long trunc, double* vals)
{
	long i = 0, numVals;
	static double gvals[] =  {     2.4048255577E0,   5.5200781103E0,
			8.6537279129E0,   11.7915344391E0,  14.9309177086E0,
			18.0710639679E0,  21.2116366299E0,  24.3524715308E0,
			27.4934791320E0,  30.6346064684E0,  33.7758202136E0,
			36.9170983537E0,  40.0584257646E0,  43.1997917132E0,
			46.3411883717E0,  49.4826098974E0,  52.6240518411E0,
			55.7655107550E0,  58.9069839261E0,  62.0484691902E0,
			65.1899648002E0,  68.3314693299E0,  71.4729816036E0,
			74.6145006437E0,  77.7560256304E0,  80.8975558711E0,
			84.0390907769E0,  87.1806298436E0,  90.3221726372E0,
			93.4637187819E0,  96.6052679510E0,  99.7468198587E0,
			102.8883742542E0, 106.0299309165E0, 109.1714896498E0,
			112.3130502805E0, 115.4546126537E0, 118.5961766309E0,
			121.7377420880E0, 124.8793089132E0, 128.0208770059E0,
			131.1624462752E0, 134.3040166383E0, 137.4455880203E0,
			140.5871603528E0, 143.7287335737E0, 146.8703076258E0,
			150.0118824570E0, 153.1534580192E0, 156.2950342685E0, };

	numVals = NUMBER(gvals);
	for( i = 0; i < trunc; i++)
	{
		if(i < numVals)
			vals[i] = gvals[i];
		else
			vals[i] = vals[i-1] +  M_PI;
	}
}

/* 'trunc' is the Gaussian number (or order) */
/* i.e. Number of parallels between a pole and the equator */
int grib_get_gaussian_latitudes(long trunc, double *lats)
{
	long jlat, iter, legi;
	double rad2deg, convval, root, legfonc = 0;
	double mem1, mem2, conv;
	double denom = 0.0;
	double precision = 1.0E-14;
	long  nlat = trunc*2;

	rad2deg = 180.0/M_PI;

	convval   = (1.0 - ((2.0 / M_PI)*(2.0 / M_PI)) * 0.25);

	gauss_first_guess(trunc, lats);
	denom = sqrt( ((((double)nlat)+0.5)*(((double)nlat)+0.5)) + convval );

	for (jlat = 0; jlat < trunc; jlat++)
	{
		/*   First approximation for root      */
		root = cos( lats[jlat] / denom );

		/*   Perform loop of Newton iterations  */
		iter = 0;
		conv = 1;

		while(fabs(conv) >= precision )
		{
			mem2 = 1.0;
			mem1 = root;

			/*  Compute Legendre polynomial  */
			for(legi = 0; legi < nlat; legi++)
			{
				legfonc = ( (2.0 * (legi+1) - 1.0) * root * mem1 - legi * mem2) / ((double)(legi+1));
				mem2 = mem1;
				mem1 = legfonc;
			}

			/*  Perform Newton iteration  */
			conv = legfonc / ((((double)nlat) * (mem2 - root * legfonc) ) / (1.0 - (root *root)));
			root -= conv;

			/*  Routine fails if no convergence after MAXITER iterations  */
			if( iter++ > MAXITER )
			{
				return GRIB_GEOCALCULUS_PROBLEM;
			}
		}

		/*   Set North and South values using symmetry */
		lats[jlat] = asin(root) * rad2deg;
		lats[nlat-1-jlat] = -lats[jlat];
	}

	if( nlat != (trunc*2) )
		lats[trunc + 1] = 0.0;
	return GRIB_SUCCESS;
}
