
!Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 Implementation: bufr_read_tropical_cyclone
!
! Description: how to read data for a tropical cyclone BUFR message.
!

program bufr_read_tropical_cyclone
  use eccodes
  implicit none
  integer            :: ifile
  integer            :: iret
  integer            :: ibufr,skipMember
  integer            :: significance
  integer            :: year,month,day,hour,minute
  integer            :: i,j,k,ierr,count=1
  integer            :: rankPosition,rankSignificance,rankPressure,rankWind
  integer            :: rankPeriod,numberOfPeriods

  real(kind=8) :: latitudeCentre,longitudeCentre
  real(kind=8), dimension(:), allocatable :: latitudeMaxWind0,longitudeMaxWind0,windMaxWind0
  real(kind=8), dimension(:), allocatable :: latitudeAnalysis,longitudeAnalysis,pressureAnalysis
  real(kind=8), dimension(:,:), allocatable :: latitude,longitude,pressure
  real(kind=8), dimension(:,:), allocatable :: latitudeWind,longitudeWind,wind
  integer(kind=4), dimension(:), allocatable :: memberNumber,period
  real(kind=8), dimension(:), allocatable :: values
  integer(kind=4), dimension(:), allocatable :: ivalues

  character(len=8)   :: rankSignificanceStr,rankPositionStr,rankPressureStr,rankWindStr
  character(len=8)   :: stormIdentifier,rankPeriodStr

  call codes_open_file(ifile,'../../data/bufr/tropical_cyclone.bufr','r')

  ! The first BUFR message is loaded from file.
  ! ibufr is the BUFR id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  do while (iret/=CODES_END_OF_FILE)

    write(*,'(A,I3,A)') '**************** MESSAGE: ',count,'  *****************'

    ! We need to instruct ecCodes to unpack the data values
    call codes_set(ibufr,"unpack",1);

    call codes_get(ibufr,'year',year);
    call codes_get(ibufr,'month',month);
    call codes_get(ibufr,'day',day);
    call codes_get(ibufr,'hour',hour);
    call codes_get(ibufr,'minute',minute);
    write(*,'(A,I0,A,I0,A,I0,A,I0,A,I0,A,I0)')'Date and time: ',day,'.',month,'.',year,'  ',hour,':',minute

    call codes_get(ibufr,'stormIdentifier',stormIdentifier)
    write(*,'(A,A)')'Storm identifier: ',stormIdentifier

    ! How many different timePeriod in the data structure?
    rankPeriod=0
    ierr=0
    do while(ierr==0)
      rankPeriod=rankPeriod+1
      write (rankPeriodStr,'(I0)')rankPeriod
      call codes_get(ibufr,'#'//trim(rankPeriodStr)//'#timePeriod',period,ierr)
      if(allocated(period)) deallocate(period)
    enddo
    ! The numberOfPeriods includes the analysis (period=0)
    numberOfPeriods=rankPeriod

    call codes_get(ibufr,'ensembleMemberNumber',memberNumber)

    allocate(latitude(size(memberNumber),numberOfPeriods))
    allocate(longitude(size(memberNumber),numberOfPeriods))
    allocate(pressure(size(memberNumber),numberOfPeriods))
    allocate(latitudeWind(size(memberNumber),numberOfPeriods))
    allocate(longitudeWind(size(memberNumber),numberOfPeriods))
    allocate(wind(size(memberNumber),numberOfPeriods))
    allocate(values(size(memberNumber)))
    allocate(period(numberOfPeriods))
    period(1)=0

    ! Observed Storm Centre
    call codes_get(ibufr,'#1#meteorologicalAttributeSignificance',significance);
    call codes_get(ibufr,'#1#latitude',latitudeCentre);
    call codes_get(ibufr,'#1#longitude',longitudeCentre);
    if (significance/=1) then
      print *,'ERROR: unexpected #1#meteorologicalAttributeSignificance'
      stop 1
    endif
    if (latitudeCentre==CODES_MISSING_DOUBLE .and. longitudeCentre==CODES_MISSING_DOUBLE) then
      write(*,'(a)')'Observed storm centre position missing'
    else
      write(*,'(A,F8.2,A,F8.2)')'Observed storm centre: latitude=',latitudeCentre,' longitude=',longitudeCentre
    endif

    ! Location of storm in perturbed analysis
    call codes_get(ibufr,'#2#meteorologicalAttributeSignificance',significance);
    call codes_get(ibufr,'#2#latitude',latitudeAnalysis);
    call codes_get(ibufr,'#2#longitude',longitudeAnalysis);
    call codes_get(ibufr,'#1#pressureReducedToMeanSeaLevel',pressureAnalysis);
    if (significance/=4) then
      print *,'ERROR: unexpected #2#meteorologicalAttributeSignificance'
      stop 1
    endif
    if (size(latitudeAnalysis)==size(memberNumber)) then
      latitude(:,1)=latitudeAnalysis
      longitude(:,1)=longitudeAnalysis
      pressure(:,1)=pressureAnalysis
    else
      latitude(:,1)=latitudeAnalysis(1)
      longitude(:,1)=longitudeAnalysis(1)
      pressure(:,1)=pressureAnalysis(1)
    endif

    ! Location of Maximum Wind
    call codes_get(ibufr,'#3#meteorologicalAttributeSignificance',significance);
    call codes_get(ibufr,'#3#latitude',latitudeMaxWind0);
    call codes_get(ibufr,'#3#longitude',longitudeMaxWind0);
    if (significance/=3) then
      print *,'ERROR: unexpected #3#meteorologicalAttributeSignificance=',significance
      stop 1
    endif
    call codes_get(ibufr,'#1#windSpeedAt10M',windMaxWind0);
    if (size(latitudeMaxWind0)==size(memberNumber)) then
      latitudeWind(:,1)=latitudeMaxWind0
      longitudeWind(:,1)=longitudeMaxWind0
      wind(:,1)=windMaxWind0
    else
      latitudeWind(:,1)=latitudeMaxWind0(1)
      longitudeWind(:,1)=longitudeMaxWind0(1)
      wind(:,1)=windMaxWind0(1)
    endif

    rankSignificance=3
    rankPosition=3
    rankPressure=1
    rankWind=1
    rankPeriod=0

    ! Loop on all periods excluding analysis period(1)=0
    do i=2,numberOfPeriods

      rankPeriod=rankPeriod+1
      write (rankPeriodStr,'(I0)')rankPeriod
      call codes_get(ibufr,'#'//trim(rankPeriodStr)//'#timePeriod',ivalues);
      do k=1,size(ivalues)
        if (ivalues(k)/=CODES_MISSING_LONG) then
          period(i)=ivalues(k)
          exit
        endif
      enddo
      deallocate(ivalues)

      ! Location of the storm
      rankSignificance=rankSignificance+1
      write (rankSignificanceStr,'(I0)')rankSignificance
      call codes_get(ibufr,'#'//trim(rankSignificanceStr)//'#meteorologicalAttributeSignificance',ivalues);
      do k=1,size(ivalues)
        if (ivalues(k)/=CODES_MISSING_LONG) then
          significance=ivalues(k)
          exit
        endif
      enddo
      deallocate(ivalues)

      rankPosition=rankPosition+1
      write (rankPositionStr,'(I0)')rankPosition
      call codes_get(ibufr,'#'//trim(rankPositionStr)//'#latitude',values);
      latitude(:,i)=values
      call codes_get(ibufr,'#'//trim(rankPositionStr)//'#longitude',values);
      longitude(:,i)=values

      if (significance==1) then
        rankPressure=rankPressure+1
        write (rankPressureStr,'(I0)')rankPressure
        call codes_get(ibufr,'#'//trim(rankPressureStr)//'#pressureReducedToMeanSeaLevel',values);
        pressure(:,i)=values
      else
        print *,'ERROR: unexpected meteorologicalAttributeSignificance=',significance
        stop 1
      endif

      ! Location of maximum wind
      rankSignificance=rankSignificance+1
      write (rankSignificanceStr,'(I0)')rankSignificance
      call codes_get(ibufr,'#'//trim(rankSignificanceStr)//'#meteorologicalAttributeSignificance',ivalues);
      do k=1,size(ivalues)
        if (ivalues(k)/=CODES_MISSING_LONG) then
          significance=ivalues(k)
          exit
        endif
      enddo
      deallocate(ivalues)

      rankPosition=rankPosition+1
      write (rankPositionStr,'(I0)')rankPosition
      call codes_get(ibufr,'#'//trim(rankPositionStr)//'#latitude',values);
      latitudeWind(:,i)=values
      call codes_get(ibufr,'#'//trim(rankPositionStr)//'#longitude',values);
      longitudeWind(:,i)=values

      if (significance==3) then
        rankWind=rankWind+1
        write (rankWindStr,'(I0)')rankWind
        call codes_get(ibufr,'#'//trim(rankWindStr)//'#windSpeedAt10M',values);
        wind(:,i)=values
      else
        print *,'ERROR: unexpected meteorologicalAttributeSignificance=,',significance
        stop 1
      endif

    enddo

    ! Print the values
    do i=1,size(memberNumber)
      skipMember=1
      do j=1,size(period)
        if (latitude(i,j)/=CODES_MISSING_DOUBLE .OR. latitudeWind(i,j)/=CODES_MISSING_DOUBLE) then
          skipMember=0
          exit
        endif
      enddo
      if (skipMember/=1) then

        write(*,'(A,I3)') '== Member ',memberNumber(i)
        write(*,*) 'step  latitude   longitude   pressure    latitude   longitude   wind'
        do j=1,size(period)
          if (latitude(i,j)/=CODES_MISSING_DOUBLE .OR. latitudeWind(i,j)/=CODES_MISSING_DOUBLE) then
          write(*,'( I4,2X,F8.2,4X,F8.2,3X,F9.1,2X,F8.2,4X,F8.2,2X,F8.2)') period(j),latitude(i,j),longitude(i,j),pressure(i,j),&
          &latitudeWind(i,j),longitudeWind(i,j),wind(i,j)
          endif
        enddo

      endif
    enddo

    ! deallocating the arrays is very important
    ! because the behaviour of the codes_get functions is as follows:
    ! if the array is not allocated then allocate
    ! if the array is already allocated only copy the values
    deallocate(values)
    deallocate(latitude)
    deallocate(longitude)
    deallocate(pressure)
    deallocate(latitudeWind)
    deallocate(longitudeWind)
    deallocate(wind)
    deallocate(period)
    deallocate(latitudeAnalysis)
    deallocate(longitudeAnalysis)
    deallocate(pressureAnalysis)
    deallocate(memberNumber)
    deallocate(latitudeMaxWind0)
    deallocate(longitudeMaxWind0)
    deallocate(windMaxWind0)

    ! Release the BUFR message
    call codes_release(ibufr)

    ! Load the next BUFR message
    call codes_bufr_new_from_file(ifile,ibufr,iret)

    count=count+1

  end do

  ! Close file
  call codes_close_file(ifile)

end program bufr_read_tropical_cyclone
