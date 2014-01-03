! Copyright 2005-2014 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
!
!
program new_from_file
use grib_api
  implicit none
  integer            :: ifile
  integer            :: iret
  integer           :: count1=0
  character(len=256) :: filename

!     Message identifier.
  integer            :: igrib

  ifile=5

  call grib_open_file(ifile,'../../data/collection.grib1','r')

! Loop on all the messages in a file.

  call grib_new_from_file(ifile,igrib, iret)

  do while (iret==GRIB_SUCCESS)
    count1=count1+1
    print *, "===== Message #",count1
    call grib_new_from_file(ifile,igrib, iret)

  end do 
  if (iret /= GRIB_END_OF_FILE) then
    call grib_check(iret,'new_from_file','')
  endif


  call grib_close_file(ifile)

end program 
