!
!Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 Implementation: bufr_read_synop
!
! Description: how to read SYNOP BUFR messages.
 
! Please note that SYNOP reports can be encoded in various ways in BUFR. Therefore the code
! below might not work directly for other types of SYNOP messages than the one used in the
! example. It is advised to use bufr_dump first to understand the structure of these messages.

!
!
program bufr_read_synop
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: count=0
integer(kind=4)    :: blockNumber,stationNumber 
real(kind=8)       :: lat,t2m,td2m,ws,wdir
integer(kind=4)    :: cloudAmount,cloudBaseHeight,lowCloud,midCloud,highCloud 

  call codes_open_file(ifile,'../../data/bufr/syno_multi.bufr','r')

! the first bufr message is loaded from file
! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)

    write(*,*) 'message: ',count

    ! we need to instruct ecCodes to expand all the descriptors 
    ! i.e. unpack the data values
    call codes_set(ibufr,"unpack",1)
    
    !read and print some data values. This example was written 
    ! for a SYNOP BUFR file! 
           
    ! wmo block number
    call codes_get(ibufr,'blockNumber',blockNumber)
    write(*,*) '  blockNumber:',blockNumber
    
    ! station number
    call codes_get(ibufr,'stationNumber',stationNumber)
    write(*,*) '  stationNumber:',stationNumber
    
    ! location
    call codes_get(ibufr,'latitude',lat)
    write(*,*) '  latitude:',lat
    
    call codes_get(ibufr,'longitude',lat)
    write(*,*) '  longitude:',lat
    
    ! 2m temperature
    call codes_get(ibufr,'airTemperatureAt2M',t2m);
    write(*,*) '  airTemperatureAt2M:',t2m
   
    ! 2m dewpoint temperature
    call codes_get(ibufr,'dewpointTemperatureAt2M',td2m);
    write(*,*) '  dewpointTemperatureAt2M:',td2m
   
    ! 10m wind
    call codes_get(ibufr,'windSpeedAt10M',ws);
    write(*,*) '  windSpeedAt10M:',ws
   
    call codes_get(ibufr,'windDirectionAt10M',wdir);
    write(*,*) '  windDirectionAt10M:',wdir
   
    ! The cloud information is stored in several blocks in the
    ! SYNOP message and the same key means a different thing in different
    ! parts of the message. In this example we will read the first
    ! cloud block introduced by the key
    ! verticalSignificanceSurfaceObservations=1. 
    ! We know that this is the first occurrence of the keys we want to
    ! read so we will use the # (occurrence) operator accordingly. 
        
    ! Cloud amount (low and middleclouds)
    call codes_get(ibufr,'#1#cloudAmount',cloudAmount)
    write(*,*) '  cloudAmount (low and middle):',cloudAmount
        
    ! Height of cloud base
    call codes_get(ibufr,'#1#heightOfBaseOfCloud',cloudBaseHeight)
    write(*,*) '  heightOfBaseOfCloud:',cloudBaseHeight
        
    ! Cloud type (low clouds)
    call codes_get(ibufr,'#1#cloudType',lowCloud)
    write(*,*) '  cloudType (low):',lowCloud 
        
    ! Cloud type (middle clouds)
    call codes_get(ibufr,'#2#cloudType',midCloud)
    write(*,*) '  cloudType (middle):',midCloud 
    
    ! Cloud type (high clouds)
    call codes_get(ibufr,'#3#cloudType',highCloud)
    write(*,*) '  cloudType (high):',highCloud 
   
    ! release the bufr message
    call codes_release(ibufr)

    ! load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)
    
    count=count+1
    
  end do  

! close file  
  call codes_close_file(ifile)
 

end program bufr_read_synop

