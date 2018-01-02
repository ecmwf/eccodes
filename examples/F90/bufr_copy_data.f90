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
! FORTRAN 90 implementation: bufr_copy_data
!
! Description: How to copy all the values in the data section that are present in the same
!              position in the data tree and with the same number of values to the output handle
!
!

program bufr_encode
  use eccodes
  implicit none
  integer                                       :: iret
  integer                                       :: outfile
  integer                                       :: ifile
  integer                                       :: ibufr
  integer                                       :: ibufrin
  integer(kind=4), dimension(:), allocatable    :: ivalues
  integer, parameter                            :: max_strsize = 200
  character(len=max_strsize)                    :: infile_name
  character(len=max_strsize)                    :: outfile_name

  call codes_bufr_new_from_samples(ibufr,'BUFR3',iret)
  call getarg(1, infile_name)
  call getarg(2, outfile_name)
  call codes_open_file(ifile, infile_name, 'r')
  call codes_bufr_new_from_file(ifile, ibufrin)

  if (iret/=CODES_SUCCESS) then
    print *,'ERROR creating BUFR from BUFR3'
    stop 1
  endif
  if(allocated(ivalues)) deallocate(ivalues)
  allocate(ivalues(69))
  ivalues=(/   &
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   &
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   &
      0, 0, 0, 1, 1, 1, 1, 1, 1, 1,   &
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   &
      1, 1, 1, 1, 1, 0, 1, 1, 1, 1,   &
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   &
      1, 1, 1, 1, 1, 1, 1, 1, 0 /)
  call codes_set(ibufr,'inputDataPresentIndicator',ivalues)
  call codes_set(ibufr,'edition',3)
  call codes_set(ibufr,'masterTableNumber',0)
  call codes_set(ibufr,'bufrHeaderSubCentre',0)
  call codes_set(ibufr,'bufrHeaderCentre',98)
  call codes_set(ibufr,'updateSequenceNumber',1)
  call codes_set(ibufr,'dataCategory',0)
  call codes_set(ibufr,'dataSubCategory',140)
  call codes_set(ibufr,'masterTablesVersionNumber',13)
  call codes_set(ibufr,'localTablesVersionNumber',1)
  call codes_set(ibufr,'typicalYearOfCentury',15)
  call codes_set(ibufr,'typicalMonth',5)
  call codes_set(ibufr,'typicalDay',4)
  call codes_set(ibufr,'typicalHour',9)
  call codes_set(ibufr,'typicalMinute',30)
  call codes_set(ibufr,'numberOfSubsets',1)
  call codes_set(ibufr,'observedData',1)
  call codes_set(ibufr,'compressedData',0)
  if(allocated(ivalues)) deallocate(ivalues)
  allocate(ivalues(43))
  ivalues=(/  &
    307011,7006,10004,222000,101023,31031,1031,1032,101023,33007, &
    225000,236000,101023,31031,1031,1032,8024,101001,225255,225000, &
    236000,101023,31031,1031,1032,8024,101001,225255, &
    1063,2001,4001,4002,4003,4004,4005,5002, &
    6002,7001,7006,11001,11016,11017,11002 /)
  call codes_set(ibufr,'unexpandedDescriptors',ivalues)

  call codes_set(ibufrin,'unpack',1)

  call codes_bufr_copy_data(ibufrin, ibufr)

  call codes_open_file(outfile,outfile_name,'w')
  call codes_write(ibufr,outfile)
  call codes_close_file(outfile)
  call codes_release(ibufr)
end program bufr_encode
