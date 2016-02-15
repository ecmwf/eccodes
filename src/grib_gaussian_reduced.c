/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api_internal.h"
#include <assert.h>

/*
 * C Implementation: gaussian_reduced
 *
 * Description: computes the number of points within the range
 *              lon_first->lon_last and the zero based indexes
 *              ilon_first,ilon_last of the first and last point
 *              given the number of points along a parallel (pl)
 *
 */
#define EFDEBUG 0

void grib_get_reduced_row(long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last )
{
  double range=0,dlon_first=0,dlon_last=0;
  long irange;

  range=lon_last-lon_first;
  if (range<0) {range+=360;lon_first-=360;}

  /* computing integer number of points and coordinates without using floating point resolution*/
  *npoints=(range*pl)/360.0+1;
  *ilon_first=(lon_first*pl)/360.0;
  *ilon_last=(lon_last*pl)/360.0;

  irange=*ilon_last-*ilon_first+1;

#if EFDEBUG
  printf("  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
    pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif

  if (irange != *npoints) {
#if EFDEBUG
    printf("       ---> (irange=%ld) != (npoints=%ld) ",irange,*npoints);
#endif
    if (irange > *npoints) {
      /* checking if the first point is out of range*/
      dlon_first=((*ilon_first)*360.0)/pl;
      if (dlon_first < lon_first) {(*ilon_first)++;irange--;
#if EFDEBUG
        printf(" dlon_first=%.10e < lon_first=%.10e\n",dlon_first,lon_first );
#endif
      }

      /* checking if the last point is out of range*/
      dlon_last=((*ilon_last)*360.0)/pl;
      if (dlon_last > lon_last) {(*ilon_last)--;irange--;
#if EFDEBUG
        printf(" dlon_last=%.10e < lon_last=%.10e\n",dlon_last,lon_last );
#endif
      }
    } else {
      int ok=0;
      /* checking if the point before the first is in the range*/
      dlon_first=((*ilon_first-1)*360.0)/pl;
      if (dlon_first > lon_first) {(*ilon_first)--;irange++;ok=1;
#if EFDEBUG
        printf(" dlon_first1=%.10e > lon_first=%.10e\n",dlon_first,lon_first );
#endif
      }

      /* checking if the point after the last is in the range*/
      dlon_last=((*ilon_last+1)*360.0)/pl;
      if (dlon_last < lon_last) {(*ilon_last)++;irange++;ok=1;
#if EFDEBUG
        printf(" dlon_last1=%.10e > lon_last=%.10e\n",dlon_last,lon_first );
#endif
      }

      /* if neither of the two are triggered then npoints is too large */
      if (!ok) {(*npoints)--;
#if EFDEBUG
        printf(" (*npoints)--=%ld\n",*npoints);
#endif
      }
    }

    assert(*npoints==irange);
#if EFDEBUG
	printf("--  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
		   pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif
  } else {
	  /* checking if the first point is out of range*/
	  dlon_first=((*ilon_first)*360.0)/pl;
	  if (dlon_first < lon_first) {
		  (*ilon_first)++;(*ilon_last)++;
#if EFDEBUG
		  printf("       ---> dlon_first=%.10e < lon_first=%.10e\n",dlon_first,lon_first );
		  printf("--  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
				 pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif
	  }

  }

  if (*ilon_first<0) *ilon_first+=pl;

  return;
}
