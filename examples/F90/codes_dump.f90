! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program codes_dump_test
   use eccodes
   implicit none
   integer            :: ifile, iret, msgid
   integer            :: product_kind
   character(len=10)  :: product_string
   character(len=512) :: infile_name

   call getarg(1, product_string)
   product_kind = CODES_PRODUCT_GRIB
   if (product_string == "bufr") product_kind = CODES_PRODUCT_BUFR

   call getarg(2, infile_name)

   call codes_open_file(ifile, infile_name, 'r')

   print *, "===== FILE:", infile_name
   do while (.true.)
      call codes_new_from_file(ifile, msgid, product_kind, iret)
      !call codes_any_new_from_file(ifile, msgid, iret)
      if (iret == CODES_END_OF_FILE) exit

      call codes_dump(msgid)

      call codes_release(msgid)

   end do

   call codes_close_file(ifile)

end program
