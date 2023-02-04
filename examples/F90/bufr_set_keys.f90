! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: How to set different type of keys in BUFR messages.
!
!
program bufr_set_keys
   use eccodes
   implicit none
   integer                                       :: iret
   integer                                       :: infile, outfile
   integer                                       :: ibufr
   integer                                       :: count = 0
   integer(kind=4)                               :: centre, centreNew

   ! Open input file
   call codes_open_file(infile, '../../data/bufr/syno_multi.bufr', 'r')

   ! Open output file
   call codes_open_file(outfile, 'bufr_set_keys_test_f.tmp.bufr', 'w')

   ! The first bufr message is loaded from file,
   ! ibufr is the bufr id to be used in subsequent calls
   call codes_bufr_new_from_file(infile, ibufr, iret)

   do while (iret /= CODES_END_OF_FILE)

      write (*, *) 'message: ', count

      ! This is the place where you may wish to modify the message
      ! E.g. we change the centre

      ! Set centre
      centre = 222
      call codes_set(ibufr, 'bufrHeaderCentre', 222)
      write (*, *) '  set bufrHeaderCentre to:', centre

      ! Check centre's new value
      centreNew = 0
      call codes_get(ibufr, 'bufrHeaderCentre', centreNew)
      write (*, *) '  bufrHeaderCentre''s new value:', centreNew

      ! Write modified message to a file
      call codes_write(ibufr, outfile)

      ! Release the handle
      call codes_release(ibufr)

      ! Next message from source
      call codes_bufr_new_from_file(infile, ibufr, iret)

      count = count + 1

   end do

   call codes_close_file(infile)
   call codes_close_file(outfile)

end program bufr_set_keys
