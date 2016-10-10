!
!Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 Implementation: bufr_get_string_array
!
! Description: how to get an array of strings from a BUFR message.
 
program bufr_get_string_array
  use eccodes
  implicit none
  integer            :: ifile
  integer            :: iret,i,n
  integer            :: ibufr
  integer            :: strsize
  integer, parameter  :: max_strsize = 20
  character(len=max_strsize) , dimension(:),allocatable   :: stationOrSiteName 

  call codes_open_file(ifile,'../../data/bufr/pgps_110.bufr','r')

  call codes_bufr_new_from_file(ifile,ibufr,iret)

  ! unpack the data values
  call codes_set(ibufr,'unpack',1)

  ! get the width of the strings which is the same for all of them
  call codes_get(ibufr,'stationOrSiteName->width',strsize)

  ! the width is given in bits
  strsize=strsize/8

  ! max_strsize has to be set to a value >= to the size of the strings that we are getting 
  ! back from the call to codes_get_string_array
  if (strsize > max_strsize) then
    print *,'stationOrSiteName array dimension is ',max_strsize,' and should be ',strsize
    call exit(1)
  end if

  ! allocating the array of strings to be passed to codes_get_string_array is mandatory
  call codes_get_size(ibufr,'stationOrSiteName',n)
  allocate(stationOrSiteName(n))

  ! passing an array of strings stationOrSiteName which must be allocated beforehand 
  call codes_get_string_array(ibufr,'stationOrSiteName',stationOrSiteName)
  do i=1,n
    write(*,'(A)')trim(stationOrSiteName(i))
  end do

  !remember to deallocate 
  deallocate(stationOrSiteName)

  ! release memory associated with bufr handle
  ! ibufr won't be accessible after this
  call codes_release(ibufr)

  ! close file  
  call codes_close_file(ifile)

end program bufr_get_string_array
