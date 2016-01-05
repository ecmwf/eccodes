! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
!  Description: how to set a bitmap in a GRIB message
!
!
program set_bitmap
  use eccodes
  implicit none
  integer                         :: infile,outfile
  integer                         :: igrib, iret
  integer                         :: numberOfValues
  real, dimension(:), allocatable :: values
  real                            :: missingValue
  logical                         :: grib1Example

  grib1Example=.true.

  if (grib1Example) then
    ! GRIB 1 example
    call codes_open_file(infile,'../../data/regular_latlon_surface.grib1','r')
  else
    ! GRIB 2 example
    call codes_open_file(infile,'../../data/regular_latlon_surface.grib2','r')
  end if
  
  call codes_open_file(outfile,'out.set_bitmap_f.grib','w')
  
  ! A new grib message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib)
  
  ! The missingValue is not coded in the message. 
  ! It is a value we define as a placeholder for a missing value
  ! at a point in the grid.
  ! It should be chosen so that it cannot be confused 
  ! with a valid field value
  missingValue=9999
  call codes_set(igrib, 'missingValue',missingValue)
  write(*,*) 'missingValue=',missingValue

  ! get the size of the values array
  call codes_get_size(igrib,'values',numberOfValues)
  write(*,*) 'numberOfValues=',numberOfValues
  
  allocate(values(numberOfValues), stat=iret)

  ! get data values
  call codes_get(igrib,'values',values)
  
  ! enable bitmap 
  call codes_set(igrib, 'bitmapPresent', 1)

  ! set some values to be missing
  values(1:10) = missingValue

  ! set the values (the bitmap will be automatically built)
  call codes_set(igrib,'values', values)

  !  write modified message to a file
  call codes_write(igrib,outfile)
  
  ! free memory
  call codes_release(igrib)
  
  call codes_close_file(infile)
  call codes_close_file(outfile)

  deallocate(values)

end program set_bitmap
