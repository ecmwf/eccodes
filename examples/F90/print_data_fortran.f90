! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Fortran 90  Implementation: print_data_fortran
!
!  Description: prints all the data contained in a grib file
!
!
!
program print_data_fortran
use eccodes
implicit none
integer            :: ifile
integer            :: igrib
integer            :: i
real(kind=8), dimension(:), allocatable       :: values
real(kind=8)       :: average
real(kind=8)       :: max
real(kind=8)       :: min

  call codes_open_file(ifile, &
           '../../data/constant_field.grib1','r')

  ! A new grib message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(ifile,igrib)

  call codes_get(igrib,'values',values)

  do i=1,size(values)
    write(*,*)'  ',i,values(i)
  enddo

  write(*,*)size(values),' values found '

  call codes_get(igrib,'max',max)
  write(*,*) 'max=',max
  call codes_get(igrib,'min',min)
  write(*,*) 'min=',min
  call codes_get(igrib,'average',average)
  write(*,*) 'average=',average

  call codes_release(igrib)

  call codes_close_file(ifile)

  deallocate(values)
end program print_data_fortran
