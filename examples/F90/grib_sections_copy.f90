! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program grib_sections_copy
   use eccodes
   implicit none

   integer :: igrib_a, igrib_b, igrib_c, iret
   character(len=40) :: error_message

   call codes_grib_new_from_samples(igrib_a, 'regular_ll_sfc_grib1')
   call codes_grib_new_from_samples(igrib_b, 'reduced_gg_pl_640_grib1')

   ! Turn all bits on:
   ! GRIB_SECTION_PRODUCT, GRIB_SECTION_GRID, GRIB_SECTION_LOCAL
   ! GRIB_SECTION_DATA, GRIB_SECTION_BITMAP
   call codes_grib_util_sections_copy(igrib_a, igrib_b, 31, igrib_c)

   call codes_release(igrib_a)
   call codes_release(igrib_b)
   call codes_release(igrib_c)

   ! Try from grib1 to grib2
   call codes_grib_new_from_samples(igrib_a, 'GRIB1')
   call codes_grib_new_from_samples(igrib_b, 'GRIB2')
   call codes_grib_util_sections_copy(igrib_a, igrib_b, 31, igrib_c, iret)
   if (iret /= GRIB_DIFFERENT_EDITION) then
      call codes_check(CODES_INTERNAL_ERROR, 'Error', 'codes_grib_util_sections_copy: Should have failed')
   end if
   call codes_get_error_string(iret, error_message)
   write (*, *) 'error message: ', adjustl( adjustr(trim(error_message)) )

end program
