! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!> Module grib_api
!>
!> The grib_api module provides the Fortran 90 interface of the GRIB API.
module grib_api

  implicit none
  include "grib_kinds.h"
  include "grib_api_constants.h"
  include "grib_api_externals.h"
  include "grib_api_visibility.h"

  real(8),    parameter,public :: GRIB_MISSING_DOUBLE = -1.D+100
  integer(4), parameter,public :: GRIB_MISSING_LONG   = 2147483647


  !> Create a new message in memory from an integer or character array containting the coded message.
  !>
  !> The message can be accessed through its gribid and it will be available\n
  !> until @ref grib_release is called. A reference to the original coded\n
  !> message is kept in the new message structure.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> \b Examples: \ref grib_copy_message.f90 "grib_copy_message.f90"
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param message     array containing the coded message
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_new_from_message
      module procedure grib_new_from_message_int4
      module procedure grib_new_from_message_char
  end interface grib_new_from_message

  !> Get a value of specified index from an array key.
  !>
  !> Given a gribid and key name as input a value corresponding to the given index
  !> is returned. The index is zero based i.e. the first element has
  !> zero index, the second element index one and so on.
  !> If the parameter index is an array all the values correspondig to the indexes
  !> list is returned.
  !> The gribid references to a grib message loaded in memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_nearest.f90 "grib_nearest.f90"
  !>
  !> @see grib_new_from_file, grib_release, grib_get
  !>
  !> @param[in] gribid      id of the grib loaded in memory
  !> @param[in] key         key name
  !> @param[in] index       index can be a scalar or array of integer(4)
  !> @param[out] value      value can be a scalar or array of integer(4),real(4),real(8)
  !> @param[out] status     GRIB_SUCCESS if OK, integer value on error
  interface grib_get_element
    module procedure grib_get_real4_element, &
                     grib_get_real8_element, &
                     grib_get_real4_elements, &
                     grib_get_real8_elements
  end interface grib_get_element

  !> Find the nearest point/points of a given latitude/longitude point.
  !>
  !> The value in the nearest point (or the four nearest points) is returned as well as the
  !> zero based index (which can be used in @ref grib_get_element)
  !> and its distance from the given point using the following
  !> formula radius * acos( sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon1-lon2) ).
  !>
  !> If the is_lsm flag is .true. the input field gribid is considered as
  !> a land sea mask and the nearest land point is returned.\n
  !> The nearest land point among the four neighbours is:
  !> - the nearest point with land sea mask value >= 0.5
  !> - the nearest without any other condition if all the four have land sea mask value <0.5.
  !>
  !> Arrays (real(8)) of latitude/longitude can be provided to find with one call
  !> the values,indexes and distances for all the lat/lon points listed in the arrays.
  !>
  !> If a single latitude/longitude point is provided and outlat,outlon,value,distance,index
  !> are defined as arrays with four elements the lat/lon coordinates and values, distances
  !> and indexes of the four nearest points are returned.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_nearest.f90 "grib_nearest.f90"
  !>
  !> @param[in] gribid     id of the grib loaded in memory
  !> @param[in] is_lsm      .true. if the nearest land point is required otherwise .false.
  !> @param[in] inlat       latitude of the point in degrees
  !> @param[in] inlon       longitudes of the point in degrees
  !> @param[out] outlat     latitude of the nearest point in degrees
  !> @param[out] outlon     longitude of the nearest point in degrees
  !> @param[out] distance   distance between the given point and its nearest (in km)
  !> @param[out] index      zero based index
  !> @param[out] value      value of the field in the nearest point
  !> @param[out] status     GRIB_SUCCESS if OK, integer value on error
  interface grib_find_nearest
    module procedure grib_find_nearest_single, &
                     grib_find_nearest_four_single, &
                     grib_find_nearest_multiple
  end interface grib_find_nearest

  !> Get latitude/longitude and data values.
  !>
  !> Latitudes, longitudes, data values arrays are returned.
  !> They must be properly allocated by the caller and their required
  !> dimension can be obtained with \ref grib_get_size or by getting (with \ref grib_get)
  !> the value of the integer key "numberOfPoints".
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_get_data.f90 "grib_get_data.f90"
  !>
  !> @param[in] gribid       id of the grib loaded in memory
  !> @param[out] lats        latitudes array with dimension "size"
  !> @param[out] lons        longitudes array with dimension "size"
  !> @param[out] values      data values array with dimension "size"
  !> @param[out] status      GRIB_SUCCESS if OK, integer value on error
  interface grib_get_data
    module procedure grib_get_data_real4, &
                     grib_get_data_real8
  end interface grib_get_data

