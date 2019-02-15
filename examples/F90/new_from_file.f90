! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
program new_from_file
use eccodes
  implicit none
  integer           :: ifile
  integer           :: iret
  integer           :: count1=0

  ! Message identifier.
  integer            :: igrib

  ifile=5

  call codes_open_file(ifile,'../../data/collection.grib1','r')

  ! Loop on all the messages in a file.
  call codes_grib_new_from_file(ifile,igrib, iret)

  do while (iret==CODES_SUCCESS)
    count1=count1+1
    print *, "===== Message #",count1
    call codes_grib_new_from_file(ifile,igrib, iret)

  end do
  if (iret /= CODES_END_OF_FILE) then
    call codes_check(iret,'new_from_file','')
  endif


  call codes_close_file(ifile)

end program
