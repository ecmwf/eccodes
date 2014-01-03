! Copyright 2005-2014 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
!  Description: how to set key values.
!
!
!
program set
  use grib_api
  implicit none
  integer(kind = 4)    :: centre, date1
  integer              :: infile,outfile
  integer              :: igrib


  centre = 80
  call current_date(date1)
  call grib_open_file(infile, &
       '../../data/regular_latlon_surface_constant.grib1','r')

  call grib_open_file(outfile, &
       'out.grib1','w')

  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call grib_new_from_file(infile,igrib)

  call grib_set(igrib,'dataDate',date1)
  !     set centre as a integer */
  call grib_set(igrib,'centre',centre)

! check if it is correct in the actual GRIB message

  call check_settings(igrib)

  !     write modified message to a file
  call grib_write(igrib,outfile)

  call grib_release(igrib)

  call grib_close_file(infile)

  call grib_close_file(outfile)

contains

!======================================
subroutine current_date(date1)
integer, intent(out) :: date1

integer              :: val_date(8)
call date_and_time ( values = val_date)

date1 = val_date(1)* 10000 + val_date(2)*100 + val_date(3) 
end subroutine current_date
!======================================
subroutine check_settings(gribid)
  implicit none
  integer, intent(in) :: gribid
  
  integer(kind = 4)    :: int_value
  character(len = 10)  :: string_value

  !     get centre as a integer
  call grib_get(gribid,'centre',int_value)
  write(*,*) "get centre as a integer - centre = ",int_value
  
  !     get centre as a string
  call grib_get(gribid,'centre',string_value)
  write(*,*) "get centre as a string  - centre = ",string_value
  
  !     get date as a string
  call grib_get(gribid,'dataDate',string_value)
  write(*,*) "get date as a string    - date = ",string_value
  
end subroutine check_settings
end program set
