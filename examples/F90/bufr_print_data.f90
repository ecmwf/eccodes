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
! FOTRAN 90 Implementation: bufr_print_data
!
! Description: how to read/print data values from synop BUFR messages.
!
!
program bufr_print_data
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: count=0
integer(kind=4)    :: blockNumber,stationNumber 
real(kind=8)       :: t2m

  call codes_open_file(ifile,'../../data/bufr/syno_multi.bufr','r')

! the first bufr message is loaded from file
! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)
    
    ! get and print some keys form the BUFR header 
    write(*,*) 'message: ',count

    ! we need to instruct ecCodes to expand all the descriptors 
    ! i.e. unpack the data values
    call codes_set(ibufr,"unpack",1);
    
    ! get wmo block number
    call codes_get(ibufr,'blockNumber',blockNumber);
    write(*,*) '  blockNumber:',blockNumber
    
    ! get station number
    call codes_get(ibufr,'stationNumber',stationNumber);
    write(*,*) '  stationNumber:',stationNumber
    
    ! in the current BUFR message this key represents the 2m temperature. 
    ! it might not be available in other type of SYNOP messages
    call codes_get(ibufr,'airTemperatureAt2M',t2m);
    write(*,*) '  airTemperatureAt2M:',t2m
   
    ! release the bufr message
    call codes_release(ibufr)

    ! load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)
    
    count=count+1
    
  end do  

! close file  
  call codes_close_file(ifile)
 

end program bufr_print_data
