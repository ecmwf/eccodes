! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program grib_get_elements
   use eccodes
   implicit none

   integer :: infile, igrib, i
   real(4) :: value_real4
   real(8) :: value_real8
   real(4) :: values_real4(4)
   real(8) :: values_real8(4)
   integer :: array_of_indexes(4)

   call codes_open_file(infile, '../../data/reduced_gaussian_pressure_level.grib1', 'r')
   call codes_grib_new_from_file(infile, igrib)

   array_of_indexes = [1, 0, 2, 4]

   print *,'Values as REAL(4) at specific indexes'
   call codes_get_element(igrib, "values", array_of_indexes, values_real4)
   do i = 1, 4
      print *, array_of_indexes(i), values_real4(i)
   end do

   print *,'Values as REAL(8) at specific indexes'
   call codes_get_element(igrib, "values", array_of_indexes, values_real8)
   do i = 1, 4
      print *, array_of_indexes(i), values_real8(i)
   end do

   i = 12
   print *,'The Value REAL(4) at index=',i
   call codes_get_element(igrib, "values", i, value_real4)
   print *, value_real4

   print *,'The Value REAL(8) at index=',i
   call codes_get_element(igrib, "values", i, value_real8)
   print *, value_real8

   call codes_release(igrib)
   call codes_close_file(infile)

end program
