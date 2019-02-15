! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: prints all the data contained in a GRIB file
!
!
!
program print_data
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: igrib
integer            :: i
real(kind=8), dimension(:), allocatable       :: values
integer(kind=4)    :: numPoints
real(kind=8)       :: average
real(kind=8)       :: the_max
real(kind=8)       :: the_min

  call codes_open_file(ifile, &
           '../../data/constant_field.grib1','r')

  ! A new GRIB message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(ifile,igrib)

  ! Get the size of the values array
  call codes_get_size(igrib,'values',numPoints)

  ! Get data values
  print*, 'number of points ', numPoints
  allocate(values(numPoints), stat=iret)

  call codes_get(igrib,'values',values)

  do i=1,numPoints
    write(*,*)'  ',i,values(i)
  enddo

  write(*,*)numPoints,' values found '

  call codes_get(igrib,'max',the_max)
  write(*,*) 'max=',the_max
  call codes_get(igrib,'min',the_min)
  write(*,*) 'min=',the_min
  call codes_get(igrib,'average',average)
  write(*,*) 'average=',average

  call codes_release(igrib)
  call codes_close_file(ifile)
  deallocate(values)

end program print_data
