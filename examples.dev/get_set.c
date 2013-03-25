/*
 * C Implementation: get_set
 *
 * Description:
 *   Example on how to get and set values through key names
 *   from a grib_handle created by an message in a file.
 *   All angles are espressed in Degrees though in the
 *   message can be in millidegrees or microdegrees depending
 *   on the grib edition number.
 *   Only angle values in Degrees should be setted and getted
 *   by the user. Expert users can access the hard coded values
 *   in the grib message omitting the "InDegrees" suffix, but
 *   it's worth to notice that units are those appropriate for
 *   the message edition.
 *   WARNING: Some variables are dependent.
 *     For example numberOfPointsAlongAMeridian depends on:
 *       - jDirectionIncrementInDegrees
 *       - latitudeOfFirstGridPointInDegrees
 *       - latitudeOfLastGridPointInDegrees.
 *     Thus only these last three must be set and the first will
 *     be available. Inversely if numberOfPointsAlongAMeridian
 *     is set then jDirectionIncrementInDegrees will be
 *     automatically calculated and modified in the grib
 *     message.
 *     jDirectionIncrementInDegrees, latitudeOfFirstGridPointInDegrees,
 *     latitudeOfLastGridPointInDegrees are required variables and
 *     numberOfPointsAlongAMeridian is optional. All the required
 *     variables must be set when creating a new handle, optional
 *     ones can be set or not. If an optional variable is set other
 *     variables are automatically set. Only required variables
 *     should be set by a user. Only expert users should access
 *     optional variables and only in particulare cases.
 *     A list of all the available required keys for a message
 *     can be obtained using the command line tool grib_dump
 *     on the message file, or with the keys_iterator function
 *     (see example keys_iterator.c).
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>, (C) 2006
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include <stdio.h>
#include <unistd.h>
#include "grib_api.h"



int main(int argc, char *argv[])
{
  int ret=0;

  grib_handle* h;
  double* values;
  long numberOfPointsAlongAParallel,numberOfPointsAlongAMeridian,numberOfPoints,i,j,k;

  /* Option flags used by grib_dump.
   * Turn on:
   *          GRIB_DUMP_FLAG_VALUES to dump values
   *          GRIB_DUMP_FLAG_OPTIONAL to dump optional keys
   *          GRIB_DUMP_FLAG_READ_ONLY to dump read_only keys
   */
  int option_flags = GRIB_DUMP_FLAG_VALUES
                    | GRIB_DUMP_FLAG_OPTIONAL
                    | GRIB_DUMP_FLAG_READ_ONLY;

  h = grib_handle_new_from_template(0,"GRIB1");
  if(!h) {
    printf("ERROR: Unable to create grib handle\n");
    exit(1);
  }

  /* GRIB_CHECK is a macro that translates the error code in a message
   * printed on the standard error and exits returning the error code
   */
  GRIB_CHECK(grib_set_double(h,"latitudeOfFirstGridPointInDegrees",10),0);
  GRIB_CHECK(grib_set_double(h,"longitudeOfFirstGridPointInDegrees",-10),0);

  GRIB_CHECK(grib_set_double(h,"latitudeOfLastGridPointInDegrees",-10),0);
  GRIB_CHECK(grib_set_double(h,"longitudeOfLastGridPointInDegrees",10),0);

  GRIB_CHECK(grib_set_double(h,"jDirectionIncrementInDegrees",0.5),0);
  GRIB_CHECK(grib_set_double(h,"iDirectionIncrementInDegrees",0.5),0);

  GRIB_CHECK(grib_get_long( h,"numberOfPointsAlongAParallel",&numberOfPointsAlongAParallel),0);
  GRIB_CHECK(grib_get_long( h,"numberOfPointsAlongAMeridian",&numberOfPointsAlongAMeridian),0);

  numberOfPoints=numberOfPointsAlongAMeridian*numberOfPointsAlongAParallel;
  values=(double*)malloc(numberOfPoints*sizeof(double));

  for (j=0;j<numberOfPointsAlongAMeridian;j++) {
    for (i=0;i<numberOfPointsAlongAParallel;i++) {
      k=i+numberOfPointsAlongAParallel*j;
      values[k]=50;
    }
  }

  GRIB_CHECK(grib_set_double_array( h,"values",values,numberOfPoints),0);

  if (h) {
    /* dump key/values pairs according the option_flags and with
     * the "serialize" style. Other available styles are:
     *  - "debug"   Dump all keys with octect position
     *  - "c_code"  Dump C code to build the grib file from scratch
     */
    grib_dump_content(h,stdout,"serialize",option_flags,NULL);
  } else  {
    printf("Error: unable to create grib_handle\n");
  }
  return ret;

}
