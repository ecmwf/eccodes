!
!Copyright 2005-2017 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 implementation: bufr_keys_iterator
!
!
! Description: how to use keys_iterator functions and the
!              codes_keys_iterator structure to get all the available
!              keys in a BUFR message.
!
!
program bufr_keys_iterator
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: count=0
character(len=20)  :: name_space
character(len=256) :: key
integer            :: kiter

  call codes_open_file(ifile,'../../data/bufr/syno_1.bufr','r')

  ! name_space='' to get all the keys 
  name_space='ls'

  ! The first bufr message is loaded from file,
  ! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret /= CODES_END_OF_FILE)

    ! Get and print some keys form the BUFR header 
    write(*,*) 'message: ',count

    ! Create key iterator
    call codes_keys_iterator_new(ibufr,kiter,name_space,iret)

    if (iret .ne. 0) then
        write(*,*) 'ERROR: Unable to create keys iterator'
        call exit(1)
    end if   

    ! Get first key
    call codes_keys_iterator_next(kiter, iret)

    ! Loop over keys
    do while (iret == CODES_SUCCESS) 
    
        ! Print key name
        call codes_keys_iterator_get_name(kiter,key)
        write(*,*) '  ',trim(key)
        
        ! Get next key
        call codes_keys_iterator_next(kiter, iret)
         
    end do
      
    ! Delete key iterator 
    call codes_keys_iterator_delete(kiter)

    ! Release the bufr message
    call codes_release(ibufr)

    ! Load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)

    count=count+1

  end do

  ! Close file  
  call codes_close_file(ifile)

end program bufr_keys_iterator
