! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: How to decode GRIB2 multi-field messages.
!               Try to turn multi support on and off to
!               see the difference. Default is OFF.
!               For all the tools (e.g., grib_ls etc) multi support is ON.
!
program multi
   use eccodes
   implicit none

   integer          :: iret, counter=0
   integer(kind=4)  :: step
   integer          :: ifile, igrib

   call codes_open_file(ifile, '../../data/multi_created.grib2', 'r')

   ! Turn off support for multi-field messages
   call codes_grib_multi_support_off()

   ! Turn on support for multi-field messages
   call codes_grib_multi_support_on()

   ! Loop on all the messages in a file
   write (*, *) 'step'
   do while (.true.)
      call codes_grib_new_from_file(ifile, igrib, iret)
      if (iret == CODES_END_OF_FILE) exit
      counter = counter + 1

      call codes_get(igrib, 'step', step)
      write (*, '(i3)') step
   end do
   call codes_close_file(ifile)
   !write(*,*) 'Message count =', counter

end program multi
