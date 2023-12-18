! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program codes_f90_misc
   use eccodes
   implicit none

   character(len=64) :: error_message = ''

   call codes_gribex_mode_on()
   call codes_gribex_mode_off()

   call codes_bufr_multi_element_constant_arrays_on()
   call codes_bufr_multi_element_constant_arrays_off()

   call codes_get_error_string(-26, error_message)
   write (*, *) 'error message: ', trim(error_message)

end program
