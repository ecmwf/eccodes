! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: How to decode GRIB messages containing multiple
!               fields. Try to turn on and off multi support to
!               see the difference. Default is OFF.
!               For all the tools default is multi support ON.
!
!
program multi
  use eccodes
  implicit none
  
  integer              :: iret
  integer(kind = 4)    ::  parameterCategory,parameterNumber,discipline
  integer              :: ifile,igrib

  call codes_open_file(ifile, '../../data/multi.grib2','r')

  ! turn on support for multi fields messages */
  call codes_grib_multi_support_on()

  ! turn off support for multi fields messages */
  !call codes_grib_multi_support_off()

  call codes_grib_new_from_file(ifile,igrib)
  ! Loop on all the messages in a file.

  do while (igrib .ne. -1)

  ! get as a integer*4
  call codes_get(igrib,'discipline',discipline)
  write(*,*) 'discipline=',discipline

  ! get as a integer*4
  call codes_get(igrib,'parameterCategory', &
       parameterCategory)
  write(*,*) 'parameterCategory=',parameterCategory

  ! get as a integer*4
  call codes_get(igrib,'parameterNumber', &
       parameterNumber)
  write(*,*) 'parameterNumber=',parameterNumber

  if ( discipline .eq. 0 .and. parameterCategory .eq. 2) then
    if (parameterNumber .eq. 2) then
      write(*,*) "-------- u -------"
    endif
    if (parameterNumber .eq. 3) then
      write(*,*) "-------- v -------"
    endif
  endif

  call codes_release(igrib)
  call codes_grib_new_from_file(ifile,igrib, iret)

  end do
  call codes_close_file(ifile)

end program multi
