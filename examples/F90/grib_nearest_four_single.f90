! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
program find_nearest_4single
   use eccodes
   implicit none
   integer :: infile, i
   integer :: igrib
   real(8) :: inlat = 5, inlon = 10
   real(8) :: outlats(4), outlons(4)
   real(8) :: values(4), distances(4)
   integer(kind=kindOfInt)  :: indexes(4)

   call codes_open_file(infile, '../../data/reduced_gaussian_lsm.grib1', 'r')
   call codes_grib_new_from_file(infile, igrib)

   call codes_grib_find_nearest_four_single(igrib, .true., inlat, inlon, outlats, outlons, values, distances, indexes)
   call codes_release(igrib)

   call codes_close_file(infile)

   print *, '   outlats   outlons  values    distances  indexes'
   do i = 1, 4
      write (*, '(F10.3, F10.3, F10.5, F10.3, I8)') outlats(i), outlons(i), values(i), distances(i), indexes(i)
   end do
end program
