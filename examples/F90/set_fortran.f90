! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
! Description: how to set key values.
!
!
program set
  use eccodes
  implicit none

  integer(kind = 4)    :: centre
  integer(kind = 4)    :: int_value
  character(len = 10)  :: string_value
  character(len = 20)  :: string_centre
  integer              :: infile,outfile
  integer              :: igrib

  infile=5
  outfile=6

  call codes_open_file(infile, &
       '../../data/regular_latlon_surface_constant.grib1','r')

  call codes_open_file(outfile, &
       '../../data/out.grib1','w')

  ! A new grib message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib)

  ! set centre as a long */
  centre=80
  call codes_set(igrib,'centre',centre)

  ! get centre as a integer*4
  call codes_get(igrib,'centre',int_value)
  write(*,*) 'centre=',int_value

  ! get centre as a string
  call codes_get(igrib,'centre',string_value)
  string_centre='centre='//string_value
  write(*,*) string_centre

  ! write modified message to a file
  call codes_write(igrib,outfile)

  call codes_release(igrib)

  call codes_close_file(infile)

  call codes_close_file(outfile)

end program set
