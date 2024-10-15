! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: how to create a new GRIB message by cloning
!               an existing message.
!
!  Author:  Cristian D. Codorean
!
!
program clone
  use grib_api
  implicit none
  integer                                       :: err,i,iret
  integer                                       :: infile,outfile
  integer                                       :: n,stat
  integer                                       :: source_id,clone_id
  integer                                       :: bla

  character(len=100)                            :: grib_file
  character(len=100)                            :: out;

  call getarg(1,grib_file)
  call getarg(2,out)

  call grib_multi_support_on()

  call grib_open_file(infile,grib_file,'r')
  call grib_open_file(outfile,out,'w')

  ! count the messages in the file
  call grib_count_in_file(infile,n,stat)

  do i=1,n
    call grib_new_from_file(infile,source_id)
    call grib_clone(source_id,clone_id)
    call grib_write(clone_id,outfile)
    call grib_release(clone_id)
  end do

  call grib_release(source_id)

  call grib_close_file(infile)
  call grib_close_file(outfile)

end program clone
