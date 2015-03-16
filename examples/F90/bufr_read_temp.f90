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
!
program bufr_print_data
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: i, count=0
integer(kind=4)    :: numSigT
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
       
    ! We also make use of the fact that in our BUFR message
    ! verticalSoundingSignificance is always followed by geopotential, 
    ! airTemperature, dewpointTemperature,
    ! windDirection, windSpeed and pressure. 

    ! ---- Get the number of the temperature significant levels ----

    ! We find out the number of temperature significant levels by 
    ! counting how many pressure values we have on these levels.
      
    keyName="/verticalSoundingSignificance=4/pressure"
    
    call codes_get_size(ibufr,keyName,numSigT);

    write(*,*) "Number of temperature significant levels:", numSigT

    ! Allocate memory for the values to be read. Each 
    ! parameter must have the same number of values.
    allocate(presVal(numSigT), stat=iret);
    allocate(geoVal(numSigT), stat=iret);
    allocate(tVal(numSigT), stat=iret);
    allocate(tdVal(numSigT), stat=iret);
    
    ! ---- Get pressure ---------------------------
        
    keyName="/verticalSoundingSignificance=4/pressure"
   
    ! get the value
    call codes_get(ibufr,keyName,presVal);

    ! ---- Get gepotential ------------------------
 
    keyName="/verticalSoundingSignificance=4/geopotential"

    !Check the size
    call codes_get_size(ibufr,keyName,num)
    if (num /= numSigT) then
    
      write(*,*) "inconsistent number of geopotential values found!"
      call exit(1)
    end if   

    ! get the values
    call codes_get(ibufr,keyName,geoVal)

    ! ---- Get temperature --------------------------------
 
    keyName="/verticalSoundingSignificance=4/airTemperature"

    !Check the size
    call codes_get_size(ibufr,keyName,num)
    if (num /= numSigT) then
    
      write(*,*) "inconsistent number of temperature values found!"
      call exit(1)
    end if   

    ! get the values
    call codes_get(ibufr,keyName,tVal)    
     
    ! ---- Get dew point temperature  -----------------------
  
    keyName="/verticalSoundingSignificance=4/dewpointTemperature"

    !Check the size
    call codes_get_size(ibufr,keyName,num)
    if (num /= numSigT) then
    
      write(*,*) "inconsistent number of dew point temperature values found!"
      call exit(1)
    end if   

    ! get the values
    call codes_get(ibufr,keyName,tdVal)        
     
    ! ---- Print the values --------------------------------
    
    write(*,*) 'level    pres    geo    t    td'
    write(*,*) "--------------------------------------" 
     
    do i=1,numSigT 
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
 

end program bufr_print_data
