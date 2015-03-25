!
!Copyright 2005-2015 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FOTRAN 90 Implementation: bufr_read_temp
!
! Description: how to read temperature significant levels from TEMP BUFR messages.
!
! Please note that SYNOP reports can be encoded in various ways in BUFR. Therefore the code
! below might not work directly for other types of TEMP messages than the one used in the
! example. It is advised to bufr_dump to understand the structure of the messages.

program bufr_read_temp
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: i, count=0
integer(kind=4)    :: num
real(kind=8), dimension(:), allocatable :: presVal,geoVal,tVal ,tdVal
character(len=128)   :: keyName

  call codes_open_file(ifile,'../../data/bufr/temp_101.bufr','r')

! the first bufr message is loaded from file
! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)

    write(*,*) 'message: ',count

    ! we need to instruct ecCodes to expand all the descriptors 
    ! i.e. unpack the data values
    call codes_set(ibufr,"unpack",1);
 
    ! In what follows we rely on the fact that for 
    ! temperature significant levels the value of key 
    ! verticalSoundingSignificance is 4 (see flag table 8001 for details).
    !
    ! In our BUFR message verticalSoundingSignificance is always followed by
    !      geopotential, airTemperature, dewpointTemperature,
    !      windDirection, windSpeed and pressure. 
    ! 
    ! So in order to access any of these keys we need to use the
    ! condition: verticalSoundingSignificance=4.

    ! ---- Get pressure ---------------------------
    call codes_get(ibufr,'/verticalSoundingSignificance=4/pressure',presVal);

    ! ---- Get gepotential ------------------------
    call codes_get(ibufr,'/verticalSoundingSignificance=4/geopotential',geoVal)

    ! ---- Get temperature --------------------------------
    call codes_get(ibufr,'/verticalSoundingSignificance=4/airTemperature',tVal)    
     
    ! ---- Get dew point temperature  -----------------------
    call codes_get(ibufr,'/verticalSoundingSignificance=4/dewpointTemperature',tdVal)        

    ! ---- Check that all arrays are same size
    if (size(presVal)/=size(geoVal) .or. size(tVal)/=size(tdVal) .or. size(tdVal)/=size(presVal)) then
      print *,'inconsistent array dimension'
      exit
    endif
    num=size(presVal)
     
    ! ---- Print the values --------------------------------
    write(*,*) 'level    pres    geo    t    td'
    write(*,*) "--------------------------------------" 
     
    do i=1,num
        write(*,*) i,presVal(i),geoVal(i),tVal(i),tdVal(i)
    end do 
     
    
    ! free arrays 
    deallocate(presVal)
    deallocate(geoVal)
    deallocate(tVal)
    deallocate(tdVal) 

    ! release the bufr message
    call codes_release(ibufr)

    ! load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)
    
    count=count+1
    
  end do  

! close file  
  call codes_close_file(ifile)
 

end program bufr_read_temp
