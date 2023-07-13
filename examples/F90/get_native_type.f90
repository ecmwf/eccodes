! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSECODES_INTERNAL_ERROR.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
program get_native_type
   use eccodes
   implicit none
   integer            :: iret
   integer            :: ihandle
   integer            :: ktype = CODES_TYPE_UNDEFINED

   CALL codes_grib_new_from_samples(ihandle, "regular_ll_sfc_grib2")

   CALL codes_get_native_type(ihandle, 'edition', ktype)
   IF (ktype /= CODES_TYPE_LONG) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key edition should have integer type')
   END IF

   CALL codes_get_native_type(ihandle, 'referenceValue', ktype)
   IF (ktype /= CODES_TYPE_DOUBLE) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key referenceValue should have double type')
   END IF

   CALL codes_get_native_type(ihandle, 'identifier', ktype)
   IF (ktype /= CODES_TYPE_STRING) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key identifier should have string type')
   END IF

   CALL codes_set(ihandle, 'bitmapPresent', 1)
   CALL codes_get_native_type(ihandle, 'bitmap', ktype)
   IF (ktype /= CODES_TYPE_BYTES) THEN
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'key bitmap should have bytes type')
   END IF

   CALL codes_release(ihandle)

end program get_native_type
