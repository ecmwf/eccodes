! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: count messages in the input
!
!  Author: Cristian D. Codorean
!
!
program get
  use grib_api
  implicit none

  integer                            ::  ifile
  character(len=100)                 ::  grib_file
  integer                            ::  n,stat
  character(len=1)                   ::  multi_flag

  call getarg(1,multi_flag)
  call getarg(2,grib_file)

  if (multi_flag/="0") call grib_multi_support_on()

  call grib_open_file(ifile,grib_file,'r')

  ! count the messages in the file
  call grib_count_in_file(ifile,n,stat)

  print *,n

  call grib_close_file(ifile)
end program get
