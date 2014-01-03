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
!  Description: how to copy a message in memory
!
!
!
program copy
  use grib_api
  implicit none
  integer                            :: err, centre
  integer(kind=kindOfSize)           :: byte_size
  integer                            :: infile,outfile
  integer                            :: igrib_in,iret
  integer                            :: igrib_out
  character(len=1), dimension(:), allocatable :: message

  
  call grib_open_file(infile,'../../data/constant_field.grib1','r')
  call grib_open_file(outfile,'out.grib1','w')

  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call grib_new_from_file(infile,igrib_in)

  call grib_get_message_size(igrib_in, byte_size)
  allocate(message(byte_size), stat=err)

  call grib_copy_message(igrib_in,message)

  call grib_new_from_message(igrib_out, message)

  centre=80
  call grib_set(igrib_out,"centre",centre)

!  write messages to a file
  call grib_write(igrib_out,outfile)

  call grib_release(igrib_out)

  call grib_release(igrib_in)

  call grib_close_file(infile)
  call grib_close_file(outfile)
  deallocate(message)

end program copy
