! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
! Description: How to read values of different type of keys from BUFR messages.
!
!
program bufr_get_keys
   use eccodes
   implicit none
   integer            :: ifile
   integer            :: iret
   integer            :: ibufr
   integer            :: i, count = 0
   integer(kind=4)    :: blockNumber, stationNumber
   real(kind=8)       :: t2m
   integer(kind=4), dimension(:), allocatable    :: descriptors
   real(kind=8), dimension(:), allocatable       :: values
   character(len=9)   :: typicalDate

   call codes_open_file(ifile, '../../data/bufr/syno_multi.bufr', 'r')

   do while (.true.)
      ! A BUFR message is loaded from the file,
      ! ibufr is the BUFR id to be used in subsequent calls
      call codes_bufr_new_from_file(ifile, ibufr, iret)
      if (iret == CODES_END_OF_FILE) exit

      ! Get and print some keys from the BUFR header
      write (*, *) 'message: ', count

      ! We need to instruct ecCodes to expand all the descriptors
      ! i.e. unpack the data values
      call codes_set(ibufr, "unpack", 1);
      ! Get as character
      call codes_get(ibufr, 'typicalDate', typicalDate)
      write (*, *) '  typicalDate:', typicalDate

      ! Get as integer
      call codes_get(ibufr, 'blockNumber', blockNumber);
      write (*, *) '  blockNumber:', blockNumber

      ! Get as integer
      call codes_get(ibufr, 'stationNumber', stationNumber);
      write (*, *) '  stationNumber:', stationNumber

      ! get as real
      call codes_get(ibufr, 'airTemperatureAt2M', t2m);
      write (*, *) '  airTemperatureAt2M:', t2m

      ! ---- array of integer ----------------

      ! get the expanded descriptors
      call codes_get(ibufr, 'bufrdcExpandedDescriptors', descriptors)

      do i = 1, size(descriptors)
         write (*, *) '  ', i, descriptors(i)
      end do

      ! ---- array of real ----------------

      ! Get the expanded data values
      call codes_get(ibufr, 'numericValues', values)

      do i = 1, size(values)
         write (*, *) '  ', i, values(i)
      end do

      ! Get as character
      call codes_get(ibufr, 'typicalDate', typicalDate)
      write (*, *) '  typicalDate:', typicalDate

      ! Free arrays
      deallocate (values)
      deallocate (descriptors)

      ! Release the BUFR message
      call codes_release(ibufr)

      count = count + 1

   end do

   ! Close file
   call codes_close_file(ifile)

end program bufr_get_keys
