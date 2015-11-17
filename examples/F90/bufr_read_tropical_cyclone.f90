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
! FOTRAN 90 Implementation: bufr_read_scatterometer
!
! Description: how to read data for a given beam from scatterometer BUFR messages.
! 
! Please note that scatterometer data can be encoded in various ways in BUFR. Therefore the code
! below might not work directly for other types of messages than the one used in the
! example. It is advised to use bufr_dump first to understand the structure of these messages.


program bufr_read_tropical_cyclone
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer            :: metAttSignificance,metAttSignificance3
integer            :: year,month,day 
integer            :: i, ii,count=0
integer(kind=4)    :: numObs,nTimeSteps,rank1,rank3

real(kind=8), dimension(:), allocatable :: latVal,lonVal,pressure,wind,latVal3,lonVal3,time_steps
!integer, allocatable ::time_steps(:)
integer(kind=4), dimension(:), allocatable :: ensemble_members

character(len=32)   :: key_name_rank1,key_name_rank3,keyname

  call codes_open_file(ifile,'../../data/bufr/tropical_cyclone.bufr','r')

! the first bufr message is loaded from file
! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)

    write(*,'(A,I3)') 'message: ',count

    ! we need to instruct ecCodes to expand all the descriptors 
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

    write(*,'(A,I5)') "Number of Subsets:",numObs

    call codes_get(ibufr,'year',year);
    call codes_get(ibufr,'month',month);
    call codes_get(ibufr,'day',day);
    !allocate (time_steps(50))
    call codes_get(ibufr,'timePeriod',time_steps)
    call codes_get(ibufr,'ensembleMemberNumber',ensemble_members)

    nTimeSteps=size(time_steps)
    print*, 'time', time_steps,nTimeSteps 

    do ii=1,nTimeSteps

    rank1=(ii)*2+2  
    rank3=(ii)*2+3

    write (key_name_rank1,'(I0)')rank1 
    write (key_name_rank3,'(I0)')rank3 
    write (keyname,'(I0)')ii+1

    
    !Get latitude 
    call codes_get(ibufr,'#'//trim(key_name_rank1)//'#latitude',latVal);
     
    !Get longitude 
    call codes_get(ibufr,'#'//trim(key_name_rank1)//'#longitude',lonVal);
     
    !Get pressure 
    call codes_get(ibufr,'#'//trim(keyname)//'#pressureReducedToMeanSeaLevel',pressure);

    call codes_get(ibufr,'#'//trim(key_name_rank1)//'#meteorologicalAttributeSignificance',metAttSignificance);
    call codes_get(ibufr,'#'//trim(key_name_rank3)//'#meteorologicalAttributeSignificance',metAttSignificance3);


    !Get latitude 
    call codes_get(ibufr,'#'//trim(key_name_rank3)//'#latitude',latVal3);
     
    !Get longitude 
    call codes_get(ibufr,'#'//trim(key_name_rank3)//'#longitude',lonVal3);
     
    !Get pressure 
    call codes_get(ibufr,'#'//trim(keyname)//'#windSpeedAt10M',wind);

    
     ! ---- Check that all arrays are same size
    if (size(latVal)/= numObs .or. size(lonVal)/= numObs) then
      print *,'inconsistent array dimension'
      exit
    endif
    
    ! ---- Print the values --------------------------------
    write(*,*) "--------------------------------------" 
    write(*,'(A,I2,A,I2,A,I4,A,F3.0)')'Date:',day,'.',month,'.',year,'     Time step:',  time_steps(ii)
    write(*,'(2(A,I1) )') 'metAttSignificance:', metAttSignificance,'          metAttSignificance:', metAttSignificance3
    write(*,*) 'ens  lat    lon   pressure    lat   lon   wind10m'
     
    do i=1,numObs
        write(*,'( I4,2(F6.2,A),F8.0,A,2(F6.2,A),F4.1 )') ensemble_members(i) ,latVal(i),'  ',lonVal(i),'  ',pressure(i),'  '&
     &   ,latVal3(i),'  ',lonVal3(i),'  ',wind(i)
    end do 
     
    ! free arrays 
    deallocate(latVal)
    deallocate(lonVal)
    deallocate(pressure)
    deallocate(latVal3)
    deallocate(lonVal3)
    deallocate(wind)
!    if(allocated(time_steps))deallocate(time_steps)
!   deallocate(time_steps)

    !do i=1,15
    !time_steps(i)=-9.
    !end do

    end do

    ! release the bufr message
    call codes_release(ibufr)

    ! load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)
     
    count=count+1
    
  end do  

  ! close file  
  call codes_close_file(ifile)
 
end program bufr_read_tropical_cyclone
