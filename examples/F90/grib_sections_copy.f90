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

   integer :: igrib1, igrib2, igrib3

   call codes_grib_new_from_samples(igrib1, 'regular_ll_sfc_grib2')
   call codes_grib_new_from_samples(igrib2, 'reduced_gg_pl_640_grib2')

   ! Turn all bits on:
   ! GRIB_SECTION_PRODUCT, GRIB_SECTION_GRID, GRIB_SECTION_LOCAL
   ! GRIB_SECTION_DATA, GRIB_SECTION_BITMAP
   call codes_grib_util_sections_copy(igrib1, igrib2, 31, igrib3)

   call codes_release(igrib1)
   call codes_release(igrib2)
   call codes_release(igrib3)

end program
