! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program bufr_encode
  use eccodes
  implicit none
  integer                      :: iret, outfile
  integer                      :: ibufr
  integer, parameter           :: max_strsize = 100
  character (len=1), parameter :: missing_char = char(255)
  character(9)                 :: missing_string
  character(len=max_strsize)   :: outfile_name
  character(len=max_strsize) , dimension(:),allocatable   :: svalues

  call getarg(1, outfile_name)

  call codes_bufr_new_from_samples(ibufr,'BUFR4',iret)
  if (iret/=CODES_SUCCESS) then
    stop 1
  endif
  call codes_set(ibufr,'masterTableNumber',0)
  call codes_set(ibufr,'bufrHeaderSubCentre',0)
  call codes_set(ibufr,'bufrHeaderCentre',98)
  call codes_set(ibufr,'updateSequenceNumber',0)
  call codes_set(ibufr,'dataCategory',1)
  call codes_set(ibufr,'dataSubCategory',110)
  call codes_set(ibufr,'masterTablesVersionNumber',13)
  call codes_set(ibufr,'typicalYearOfCentury',12)
  call codes_set(ibufr,'typicalMonth',10)
  call codes_set(ibufr,'typicalDay',31)
  call codes_set(ibufr,'typicalHour',0)
  call codes_set(ibufr,'typicalMinute',2)
  call codes_set(ibufr,'numberOfSubsets',10)
  call codes_set(ibufr,'observedData',1)
  call codes_set(ibufr,'compressedData',1)

  ! Create the structure of the data section
  call codes_set(ibufr,'unexpandedDescriptors',307022)

  ! Create a string with characters all set to 255 (all bits=1)
  missing_string = repeat(missing_char, 9)

  allocate(svalues(10))
  svalues=(/"ARD2-LPTR", &
    "ETH2-LPTR", &
    "ETH2-LPTR", &
    "KALT-LPTR", &
    "KALT-LPTR", &
    "FHBB-LPTR", &
    "FHBB-LPTR", &
    "LIND-LPTR", &
    "LIND-LPTR", &
    "EPFL-LPTR" /)
  ! Change some entries to be missing (undefined)
  svalues(3) = missing_string
  svalues(10) = missing_string
  call codes_set_string_array(ibufr,'stationOrSiteName',svalues)

  ! Encode the keys back in the data section
  call codes_set(ibufr,'pack',1)

  call codes_open_file(outfile,outfile_name,'w')
  print *,'Wrote ',outfile_name
  call codes_write(ibufr,outfile)
  call codes_close_file(outfile)
  call codes_release(ibufr)

  if(allocated(svalues)) deallocate(svalues)
end program bufr_encode
