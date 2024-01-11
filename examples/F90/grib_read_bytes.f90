! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program grib_read_bytes
  use eccodes
  implicit none
  integer :: ifile
  integer :: intsize
  parameter(intsize=10000)
  character, dimension(intsize) :: bufferChar
  character(len=32)             :: input_grib_file = '../../data/v.grib2'

  call codes_open_file(ifile, input_grib_file, 'r')

  ! First 4 chars should be 'GRIB'
  call codes_read_bytes(ifile, bufferChar, 4)
  print*, bufferChar(1:5)

  call codes_close_file(ifile)

end program
