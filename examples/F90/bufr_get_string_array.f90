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
! FOTRAN 90 Implementation: bufr_get_string_array
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
  call codes_get(ibufr,'stationOrSiteName->width',strsize)
  strsize=strsize/8
  if (strsize > max_strsize) then
    print *,'stationOrSiteName array dimension is ',max_strsize,' and should be ',strsize
    call exit(1)
  end if
    
  call codes_get_size(ibufr,'stationOrSiteName',n)
  allocate(stationOrSiteName(n))
  ! passing an array of strings stationOrSiteName which must be allocated beforehand 
  call codes_get_string_array(ibufr,'stationOrSiteName',stationOrSiteName)
  do i=1,n
    write(*,'(A)')trim(stationOrSiteName(i))
  end do

  !remember to deallocate what was allocated by codes_get_string_array
  deallocate(stationOrSiteName)
    
  ! release the bufr message
  call codes_release(ibufr)

  ! close file  
  call codes_close_file(ifile)
 

end program bufr_get_string_array

