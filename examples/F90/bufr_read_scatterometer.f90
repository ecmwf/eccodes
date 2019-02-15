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
! FORTRAN 90 Implementation: bufr_read_scatterometer
!
! Description: how to read data for a given beam from scatterometer BUFR messages.
!
! Please note that scatterometer data can be encoded in various ways in BUFR. Therefore the code
! below might not work directly for other types of messages than the one used in the
! example. It is advised to use bufr_dump first to understand the structure of these messages.


program bufr_read_scatterometer
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: i, count=0
integer(kind=4)    :: numObs,ii
real(kind=8), dimension(:), allocatable :: latVal,lonVal,bscatterVal
real(kind=8), dimension(:), allocatable :: year

  call codes_open_file(ifile,'../../data/bufr/asca_139.bufr','r')

  ! The first BUFR message is loaded from file,
  ! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)

    write(*,'(A,I3)') 'message: ',count

    ! We need to instruct ecCodes to expand all the descriptors
    ! i.e. unpack the data values
    call codes_set(ibufr,"unpack",1);

    ! The BUFR file contains a single message with 2016 subsets in a compressed form.
    ! It means each subset has exactly the same structure: they store one location with
    ! several beams and one backscatter value in each beam.
    !
    ! To print the backScatter values for beamIdentifier=2 from all the subsets
    ! we will simply access the key by condition (see below).

    ! Read the total number of subsets.
    call codes_get(ibufr,'numberOfSubsets',numObs)

    write(*,'(A,I5)') "Number of values:",numObs

    ! Get latitude (for all the subsets)
    call codes_get(ibufr,'latitude',latVal);

    ! Get longitude (for all the subsets)
    call codes_get(ibufr,'longitude',lonVal);

    allocate(year(numObs))
    call codes_get(ibufr,'year',year);
    do ii= 1, size(year)
      write(*,'(A,I4,A,F8.1)') 'year(',ii,')=',year(ii)
    enddo

    ! Get backScatter for beam two. We use an access by condition for this key.
    ! (for all the subsets)
    call codes_get(ibufr,'/beamIdentifier=2/backscatter',bscatterVal);

    ! Check that all arrays are same size
    if (size(latVal)/= numObs .or. size(lonVal)/= numObs .or. size(bscatterVal)/= numObs) then
      print *,'inconsistent array dimension'
      exit
    endif

    ! Print the values
    write(*,*) 'pixel   lat    lon     backscatter'
    write(*,*) "--------------------------------------"

    do i=1,numObs
        write(*,'(I4,3F10.5)') i,latVal(i),lonVal(i),bscatterVal(i)
    end do

    ! Free arrays
    deallocate(latVal)
    deallocate(lonVal)
    deallocate(bscatterVal)

    ! Release the bufr message
    call codes_release(ibufr)

    ! Load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)

    count=count+1

  end do

  ! Close file
  call codes_close_file(ifile)

end program bufr_read_scatterometer
