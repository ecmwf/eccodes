! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: prints all the data contained in a GRIB file.
!               Using the old interface with a STATIC array
!               rather than the new ALLOCATABLE array
!
!
program print_data
use grib_api
implicit none
integer            :: ifile
integer            :: igrib
integer            :: i
real(kind=8), dimension(99200)       :: values_static
integer(kind=4)    :: numPoints
real(kind=8)       :: average
real(kind=8)       :: the_max
real(kind=8)       :: the_min

  call grib_open_file(ifile, &
           '../../data/constant_field.grib1','r')

  ! A new GRIB message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call grib_new_from_file(ifile,igrib)

  ! Get the size of the values array
  call grib_get_size(igrib,'values',numPoints)

  ! Get data values
  print*, 'number of points ', numPoints

  call grib_get(igrib,'values',values_static)

  do i=1,numPoints
    write(*,*)'  ',i,values_static(i)
  enddo

  write(*,*)numPoints,' values found '

  call grib_get(igrib,'max',the_max)
  write(*,*) 'max=',the_max
  call grib_get(igrib,'min',the_min)
  write(*,*) 'min=',the_min
  call grib_get(igrib,'average',average)
  write(*,*) 'average=',average

  call grib_release(igrib)
  call grib_close_file(ifile)

end program print_data
