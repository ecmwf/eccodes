! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 Implementation: bufr_expanded
!
! Description: How to read all the expanded data values from BUFR messages.
!
!
program bufr_expanded
   use eccodes
   implicit none
   integer            :: ifile
   integer            :: iret
   integer            :: ibufr
   integer            :: i
   integer            :: count = 0
   integer(kind=4)    :: numberOfValues
   real(kind=8), dimension(:), allocatable       :: values

   call codes_open_file(ifile, '../../data/bufr/syno_1.bufr', 'r')

   ! The first bufr message is loaded from file,
   ! ibufr is the bufr id to be used in subsequent calls
   call codes_bufr_new_from_file(ifile, ibufr, iret)

   do while (iret /= CODES_END_OF_FILE)

      write (*, *) 'message: ', count

      ! We need to instruct ecCodes to expand all the descriptors
      ! i.e. unpack the data values
      call codes_set(ibufr, "unpack", 1); 
      ! Get the expanded data values
      call codes_get(ibufr, 'numericValues', values)

      numberOfValues = size(values)

      do i = 1, numberOfValues
         write (*, *) '  ', i, values(i)
      end do

      ! Release the bufr message
      call codes_release(ibufr)

      ! Load the next bufr message
      call codes_bufr_new_from_file(ifile, ibufr, iret)

      ! Free array
      deallocate (values)

      count = count + 1

   end do

   ! Close file
   call codes_close_file(ifile)

end program bufr_expanded
