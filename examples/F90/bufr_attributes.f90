!
! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 Implementation: bufr_attributes
!
! Description: how to read attributes of keys in BUFR messages.
!
!
program bufr_attributes
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: count=0
integer(kind=4)    :: iVal,conf
real(kind=8)       :: t2m
character(len=32)   :: units, confUnits

  call codes_open_file(ifile,'../../data/bufr/syno_multi.bufr','r')

! the first bufr message is loaded from file
! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)

    ! Get and print some keys form the BUFR header
    write(*,*) 'message: ',count

    ! We need to instruct ecCodes to expand all the descriptors
    ! i.e. unpack the data values
    call codes_set(ibufr,"unpack",1);

    ! ----------------------------------------------------------------
    !  We will read the value and all the attributes available for
    !  the 2m temperature.
    ! ----------------------------------------------------------------

    ! Get the element's value as as real
    call codes_get(ibufr,'airTemperatureAt2M',t2m);
    write(*,*) '  airTemperatureAt2M:',t2m

    ! Get the element's code (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->code',iVal);
    write(*,*) '  airTemperatureAt2M->code:',iVal

    ! Get the element's units (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->units',units)
    write(*,*) '  airTemperatureAt2M->units:',units

    ! Get the element's scale (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->scale',iVal);
    write(*,*) '  airTemperatureAt2M->code:',iVal

    ! Get the element's reference (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->reference',iVal);
    write(*,*) '  airTemperatureAt2M->reference:',iVal

    ! Get the element's width (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->width',iVal);
    write(*,*) '  airTemperatureAt2M->width:',iVal

    ! -------------------------------------------------------------------
    !  The 2m temperature data element in this message has an associated
    !  field: percentConfidence. Its value and attributes can be accessed
    !  in a similar manner as was shown above for 2m temperature.
    ! -------------------------------------------------------------------

    ! Get the element's value as as real
    call codes_get(ibufr,'airTemperatureAt2M->percentConfidence',conf);
    write(*,*) '  airTemperatureAt2M->percentConfidence:', conf

    ! Get the element's code (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->percentConfidence->code',iVal);
    write(*,*) '  airTemperatureAt2M->percentConfidence->code:',iVal

    ! Get the element's units (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->percentConfidence->units',confUnits)
    write(*,*) '  airTemperatureAt2M->percentConfidence->units:',confUnits

    ! Get the element's scale (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->percentConfidence->scale',iVal);
    write(*,*) '  airTemperatureAt2M->percentConfidence->code:',iVal

    ! Get the element's reference (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->percentConfidence->reference',iVal);
    write(*,*) '  airTemperatureAt2M->percentConfidence->reference:',iVal

    ! Get the element's width (see BUFR code table B)
    call codes_get(ibufr,'airTemperatureAt2M->percentConfidence->width',iVal);
    write(*,*) '  airTemperatureAt2M->percentConfidence->width:',iVal

    ! Release the bufr message
    call codes_release(ibufr)

    ! Load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)

    count=count+1

  end do

  ! Close file
  call codes_close_file(ifile)

end program bufr_attributes
