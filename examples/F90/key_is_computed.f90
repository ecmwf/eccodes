! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSECODES_INTERNAL_ERROR.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
program key_is_computed
   use eccodes
   implicit none
   integer :: ihandle, is_computed=0

   CALL codes_grib_new_from_samples(ihandle, "regular_ll_sfc_grib2")

   CALL codes_key_is_computed(ihandle, 'edition', is_computed)
   IF (is_computed /= 0) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key edition is coded')
   END IF

   CALL codes_key_is_computed(ihandle, 'longitudeOfLastGridPoint', is_computed)
   IF (is_computed /= 0) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key longitudeOfLastGridPoint is coded')
   END IF

   CALL codes_key_is_computed(ihandle, 'longitudeOfLastGridPointInDegrees', is_computed)
   IF (is_computed /= 1) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key longitudeOfLastGridPointInDegrees is computed')
   END IF

   CALL codes_key_is_computed(ihandle, 'gridType', is_computed)
   IF (is_computed /= 1) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key gridType is computed')
   END IF

   CALL codes_release(ihandle)

end program key_is_computed
