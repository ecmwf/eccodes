! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  FORTRAN 90 implementation: bufr_clone
!
!  Description: how to create a new BUFR message by cloning 
!               an existing message.
!
!
!
program bufr_clone
  use eccodes
  implicit none
  integer       :: i,iret
  integer       :: infile,outfile
  integer       :: ibufr_in
  integer       :: ibufr_out
  
  !open source file 
  call codes_open_file(infile,'../../data/bufr/syno_multi.bufr','r')

  ! open target file
  call codes_open_file(outfile,'bufr_clone_test_f.clone.bufr','w')

  ! the first bufr message is loaded from file
  ! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(infile,ibufr_in,iret)

  ! create several clones of this message and alter them
  ! in different ways
  do i=1,3

    ! clone the current handle
    call codes_clone(ibufr_in, ibufr_out)

    ! This is the place where you may wish to modify the clone 
    ! E.g. we change the bufrHeaderCentre 
    call codes_set(ibufr_out,'bufrHeaderCentre',222)

    ! write cloned messages to a file
    call codes_write(ibufr_out,outfile)

    ! release the clone's handle
    call codes_release(ibufr_out)
  end do  

  ! release the original handle
  call codes_release(ibufr_in)

  call codes_close_file(infile)
  call codes_close_file(outfile)

end program bufr_clone
