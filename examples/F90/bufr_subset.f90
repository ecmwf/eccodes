!
!Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 implementation: bufr_subset
!
! Description: how to read data values from a given subset of a BUFR message.
!
!
program bufr_subset
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: i, count=0
integer(kind=4)    :: numberOfSubsets
integer(kind=4)    :: blockNumber,stationNumber
character(100)     :: key
!real(kind=8)       :: t2m

  call codes_open_file(ifile,'../../data/bufr/synop_multi_subset.bufr','r')

  ! The first bufr message is loaded from file,
  ! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)

    ! Get and print some keys form the BUFR header
    write(*,*) 'message: ',count

    ! We need to instruct ecCodes to expand all the descriptors
    ! i.e. unpack the data values
    call codes_set(ibufr,'unpack',1);

    ! Find out the number of subsets
    call codes_get(ibufr,'numberOfSubsets',numberOfSubsets)
    write(*,*) '  numberOfSubsets:',numberOfSubsets

    ! Loop over the subsets
    do i=1,numberOfSubsets

 100    format('/subsetNumber=',I5.5,'/blockNumber')
        write(key,100) I
        write(*,*) key

        write(*,*) ' subsetNumber:',i
        ! read and print some data values

        call codes_get(ibufr,key,blockNumber);
        write(*,*) '  blockNumber:',blockNumber

        write(key,*) '/subsetNumber=',I,'/stationNumber'
        call codes_get(ibufr,'stationNumber',stationNumber);
        write(*,*) '  stationNumber:',stationNumber

    end do

    ! Release the bufr message
    call codes_release(ibufr)

    ! Load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)

    count=count+1

  end do

  ! Close file
  call codes_close_file(ifile)

end program bufr_subset
