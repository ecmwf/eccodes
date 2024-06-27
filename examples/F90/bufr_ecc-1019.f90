! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
program operator_3_test
  use eccodes
  implicit none
  integer                                    :: iret, outfile, ibufr
  integer(kind=4), dimension(:), allocatable :: ivalues
  integer(kind=8), dimension(:), allocatable :: lvalues
  character(len=100)                         :: outfile_name

  call getarg(1, outfile_name)

  call codes_bufr_new_from_samples(ibufr,'BUFR4',iret)
  if (iret /= CODES_SUCCESS) then
    stop 1
  endif

  allocate(lvalues(1))
  lvalues=(/ -16383 /)
  call codes_set(ibufr,'inputOverriddenReferenceValues',lvalues)
  deallocate(lvalues)

  allocate(ivalues(3))
  ivalues=(/ 2,2,2 /)
  call codes_set(ibufr,'inputExtendedDelayedDescriptorReplicationFactor',ivalues)
  deallocate(ivalues)

  call codes_set(ibufr,'masterTableNumber',0)
  call codes_set(ibufr,'bufrHeaderCentre',254)
  call codes_set(ibufr,'bufrHeaderSubCentre',0)
  call codes_set(ibufr,'updateSequenceNumber',0)
  call codes_set(ibufr,'dataCategory',21)
  call codes_set(ibufr,'internationalDataSubCategory',5)
  call codes_set(ibufr,'dataSubCategory',205)
  call codes_set(ibufr,'masterTablesVersionNumber',15)
  call codes_set(ibufr,'localTablesVersionNumber',0)
  call codes_set(ibufr,'typicalYear',2017)
  call codes_set(ibufr,'typicalMonth',6)
  call codes_set(ibufr,'typicalDay',2)
  call codes_set(ibufr,'typicalHour',10)
  call codes_set(ibufr,'typicalMinute',49)
  call codes_set(ibufr,'typicalSecond',44)
  call codes_set(ibufr,'numberOfSubsets',3)
  call codes_set(ibufr,'observedData',1)
  !call codes_set(ibufr,'compressedData',1)
  call codes_set(ibufr,'compressedData',0)
  if(allocated(ivalues)) deallocate(ivalues)

  allocate(ivalues(7))
  ivalues = (/ 203015,025052,203255,025052,101000,031002,014046 /)
  ! ivalues = (/                      025052,101000,031002,014046 /)
  call codes_set(ibufr,'unexpandedDescriptors',ivalues,iret)
  if (iret /= CODES_SUCCESS) then
    write(*,*)'codes_set failed for the unexpandedDescriptors'
    stop
  endif

  deallocate(ivalues)
  ! Negative values can now be encoded
  call codes_set(ibufr, 'log10OfPrincipalComponentsNormalizedFitToData', -1.1)

  allocate(ivalues(3))
  ivalues = 0
  call codes_set(ibufr,'#1#scaledIasiRadiance',ivalues)
  call codes_set(ibufr,'#2#scaledIasiRadiance',ivalues)
  deallocate(ivalues)

  call codes_set(ibufr,'pack',1)

  call codes_open_file(outfile, outfile_name, 'w')
  call codes_write(ibufr,outfile)
  call codes_close_file(outfile)
  call codes_release(ibufr)
  if(allocated(ivalues)) deallocate(ivalues)
end program operator_3_test
