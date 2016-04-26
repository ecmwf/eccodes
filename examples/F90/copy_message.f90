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
!  Description: how to copy a GRIB message in memory
!
!
!
program copy
  use eccodes
  implicit none
  integer                            :: err, centre
  integer(kind=kindOfSize)           :: byte_size
  integer                            :: infile,outfile
  integer                            :: igrib_in
  integer                            :: igrib_out
  character(len=1), dimension(:), allocatable :: message
  character(len=32)                  :: product_kind


  call codes_open_file(infile,'../../data/constant_field.grib1','r')
  call codes_open_file(outfile,'out.copy.grib1','w')

  ! A new GRIB message is loaded from file
  ! igrib_in is the GRIB id to be used in subsequent calls
  call codes_grib_new_from_file(infile, igrib_in)

  call codes_get_message_size(igrib_in, byte_size)
  allocate(message(byte_size), stat=err)

  call codes_copy_message(igrib_in, message)

  call codes_new_from_message(igrib_out, message)

  call codes_get(igrib_out, 'kindOfProduct', product_kind)
  write(*,*) 'kindOfProduct=',product_kind

  centre=80
  call codes_set(igrib_out, 'centre', centre)

  ! Write message to a file
  call codes_write(igrib_out, outfile)

  call codes_release(igrib_out)
  call codes_release(igrib_in)
  call codes_close_file(infile)
  call codes_close_file(outfile)
  deallocate(message)

end program copy
