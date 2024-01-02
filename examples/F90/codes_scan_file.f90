! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program codes_scan_file
   use eccodes
   implicit none
   integer, parameter          :: max_strsize = 200
   integer                     :: ifile, cnt, level, step
   integer                     :: i, igrib
   character(len=max_strsize)  :: infile_name

   call getarg(1, infile_name)

   call codes_open_file(ifile, infile_name, 'r')

   call codes_any_scan_file(ifile,cnt)

   i = 45
   call codes_any_new_from_scanned_file(ifile,i,igrib)
   call codes_get(igrib, 'level', level)
   call codes_get(igrib, 'stepRange', step)

   print *, 'Num messages=', cnt
   print *, 'Msg ',i,' level=',level, ' step=', step

   call codes_release(igrib)
   call codes_close_file(ifile)

end program
