! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program set_data_force
   use eccodes
   implicit none
   integer                         :: outfile
   integer                         :: i, igrib, iret, numberOfValues, cnt
   real                            :: d, e
   real(4), dimension(:), allocatable :: values_real4
   real(8), dimension(:), allocatable :: values_real8
   integer, parameter              :: max_strsize = 200
   character(len=max_strsize)      :: outfile_name

   call getarg(1, outfile_name)
   call codes_open_file(outfile, outfile_name, 'w')

   call codes_grib_new_from_samples(igrib, 'regular_ll_pl_grib1')

   call codes_get_size(igrib, 'values', numberOfValues)

   allocate (values_real4(numberOfValues), stat=iret)
   allocate (values_real8(numberOfValues), stat=iret)
   d = 10e-8
   e = d
   cnt = 1
   do i = 1, numberOfValues
      if (cnt > 100) then
         e = e*10
         cnt = 1
      end if
      values_real4(i) = d
      values_real8(i) = d
      d = d + e
      cnt = cnt + 1
   end do

   call codes_set(igrib, 'bitsPerValue', 16)
   call codes_set(igrib, 'bitmapPresent', 1)

   ! set data values
   call codes_set_force(igrib, 'codedValues', values_real4)
   call codes_set_force(igrib, 'codedValues', values_real8)
   call codes_write(igrib, outfile)
   call codes_release(igrib)
   deallocate (values_real4)
   deallocate (values_real8)

end program set_data_force
