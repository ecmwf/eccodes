! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: How to get a GRIB message in memory
!
!
program get_message
   use eccodes
   implicit none
   integer                            :: err, centre
   integer(kind=kindOfInt)           :: byte_size
   integer(kind=kindOfInt)           :: byte_size2
   integer                            :: infile, outfile1, outfile2
   integer                            :: igrib_in
   integer                            :: igrib_out
   integer                            :: igrib_out2

   character(len=1), dimension(:), pointer :: mptr
   character(len=1), dimension(:), allocatable :: message
   character(len=32)                  :: product_kind1
   character(len=32)                  :: product_kind2

   call codes_open_file(infile, '../../data/constant_field.grib1', 'r')
   call codes_open_file(outfile1, 'temp.examples_f_grib_get_message.1.grib1', 'W')
   call codes_open_file(outfile2, 'temp.examples_f_grib_get_message.2.grib1', 'W')

   ! A new GRIB message is loaded from file
   ! igrib_in is the GRIB id to be used in subsequent calls
   call codes_grib_new_from_file(infile, igrib_in)

   call codes_get_message_size(igrib_in, byte_size)
   call grib_get_message(igrib_in, mptr, byte_size2)
   
   if(byte_size == byte_size2) then
           write(*,*) "Get message test 1 passed, message SIZES are the same!"
   else
           write(*,*) "Get message test 1 failed, message SIZES are NOT the same!"
           call abort
   endif

   allocate (message(byte_size), stat=err)
   call codes_copy_message(igrib_in, message)
   
   if(ALL(mptr == message)) then
           write(*,*) "Get message test 2 passed, message CONTENT is the same!"
   else
           write(*,*) "Get message test 2 failed, message CONTENT is NOT the same!"
           call abort     
   endif   

   call codes_new_from_message(igrib_out, message)
   call grib_new_from_message_no_copy(igrib_out2, mptr)

   call codes_get(igrib_out, 'kindOfProduct', product_kind1)
   write (*, *) 'allocatable kindOfProduct=', product_kind1
   call codes_get(igrib_out2, 'kindOfProduct', product_kind2)
   write (*, *) 'pointer kindOfProduct=', product_kind2
   if(product_kind1 == product_kind2) then
           write(*,*) "New from message (no copy) test 1 passed, GRIB_GET worked!"
   else
           write(*,*) "New from message (no copy) test 1 failed, GRIB_GET did NOT work!"
           call abort
   endif
   
   centre = 75
   call codes_set(igrib_out, 'centre', centre)
   call codes_set(igrib_out2, 'centre', centre)
   
   call codes_get(igrib_out, 'centre', centre)
   write (*, *) 'allocatable centre=', centre
   call codes_get(igrib_out2, 'centre', centre)
   write (*, *) 'pointer centre=', centre
   
  if(centre == 75) then
           write(*,*) "New from message (no copy) test 2 passed, GRIB_SET worked!"
   else
           write(*,*) "New from message (no copy) test 2 failed, GRIB_SET did NOT work!"
           call abort
   endif
   
   ! Write message to a file
   call codes_write(igrib_out, outfile1)
   call codes_write(igrib_out2, outfile2)

   call codes_release(igrib_out)
   call codes_release(igrib_in)
   call codes_release(igrib_out2)
   call codes_close_file(infile)
   call codes_close_file(outfile1)
   call codes_close_file(outfile2) 
   deallocate (message)

end program get_message
