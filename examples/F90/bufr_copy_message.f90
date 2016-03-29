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
!  Description: how to copy a BUFR message in memory
!
!
!
program copy
  use eccodes
  implicit none
  integer                            :: err, sub_centre
  integer(kind=kindOfSize)           :: byte_size
  integer                            :: infile,outfile
  integer                            :: ibufr_in
  integer                            :: ibufr_out
  character(len=1), dimension(:), allocatable :: message
  character(len=32)                  :: product_kind


  call codes_open_file(infile,'../../data/bufr/syno_1.bufr', 'r')
  call codes_open_file(outfile,'out.copy.bufr', 'w')

  ! A new BUFR message is loaded from file
  ! ibufr_in is the BUFR id to be used in subsequent calls
  call codes_bufr_new_from_file(infile, ibufr_in)

  call codes_get_message_size(ibufr_in, byte_size)
  allocate(message(byte_size), stat=err)

  call codes_copy_message(ibufr_in, message)

  call codes_new_from_message(ibufr_out, message)

  call codes_get(ibufr_out, 'kindOfProduct', product_kind)
  write(*,*) 'kindOfProduct=',product_kind

  sub_centre=80
  call codes_set(ibufr_out,'bufrHeaderSubCentre', sub_centre)

  ! Write message to a file
  call codes_write(ibufr_out, outfile)

  call codes_release(ibufr_out)
  call codes_release(ibufr_in)
  call codes_close_file(infile)
  call codes_close_file(outfile)
  deallocate(message)

end program copy
