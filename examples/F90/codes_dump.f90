! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program grib_dump_test
   use eccodes
   implicit none
   integer            :: ifile
   integer            :: iret, i
   integer            :: count1 = 0
   integer            :: msgid  ! message identifier
   character(len=100)  :: infile_name

   ifile = 5
   call getarg(1, infile_name)
   call codes_open_file(ifile, infile_name, 'r')

   do while (.true.)
      call codes_any_new_from_file(ifile, msgid, iret)
      if (iret == CODES_END_OF_FILE) exit

      count1 = count1 + 1
      print *, "===== Message #", count1

      call codes_dump(msgid)

      call codes_release(msgid)

   end do

   call codes_close_file(ifile)

end program
