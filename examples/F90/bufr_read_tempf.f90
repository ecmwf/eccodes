! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! Description: how to read radiosonde data from TEMP BUFR messages.
! This version also lists the position information from the WMO list
! (now OSCAR/Surface) - ECMWF version
!
! Author: Bruce Ingleby
!
! Please note that TEMP reports can be encoded in various ways in BUFR. Therefore the code
! below might not work directly for other types of TEMP messages than the one used in the
! example. It is advised to use bufr_dump first to understand the structure of these messages.
!
program bufr_read_temp
  use eccodes
  implicit none
  integer            :: ifile
  integer            :: iret
  integer            :: ibufr
  integer            :: i, count=0
  integer            :: iflag
  integer            :: status_id, status_ht, status_time=0, status_p
  integer            :: status_rsno, status_rssoft, statid_missing
  integer(kind=4)    :: sizews
  integer(kind=4)    :: blockNumber,stationNumber
  integer(kind=4)    :: ymd,hms
  logical            :: llstdonly = .True.  ! Set True to list standard levels only
  logical            :: llskip
  real(kind=8)       :: year,month,day,hour,minute,second
  real(kind=8)       :: htg,htp,htec=0,sondeType
  real(kind=8), dimension(:), allocatable :: lat,lon
  real(kind=8), dimension(:), allocatable :: timeVal,dlatVal,dlonVal,vssVal
  real(kind=8), dimension(:), allocatable :: presVal,zVal,tVal,tdVal,wdirVal,wspVal
  character(len=128) :: statid
  character(len=16)  :: rsnumber
  character(len=16)  :: rssoftware
  character(len=8)   :: Note

  call codes_open_file(ifile,'../../data/bufr/PraticaTemp.bufr','r')

  ! the first bufr message is loaded from file
  ! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

  ! do while (iret/=CODES_END_OF_FILE)
  do while (iret/=CODES_END_OF_FILE .AND. status_time==CODES_SUCCESS)

    ! we need to instruct ecCodes to expand all the descriptors
    ! i.e. unpack the data values
    call codes_set(ibufr,"unpack",1);

    ! In our BUFR message verticalSoundingSignificance is always followed by
    !      geopotential, airTemperature, dewpointTemperature,
    !      windDirection, windSpeed and pressure.
    !

    count=count+1
    llskip=.False.

    ! Metadata:
    call codes_get(ibufr,'shipOrMobileLandStationIdentifier',statid,status_id)
    IF (status_id /= CODES_SUCCESS) statid = "UNKNOWN"
    call codes_is_missing(ibufr,'shipOrMobileLandStationIdentifier',statid_missing)
    IF (statid_missing == 1) statid = "MISSING"
    call codes_get(ibufr,'blockNumber',blockNumber)
    call codes_get(ibufr,'stationNumber',stationNumber)
    call codes_get(ibufr,'year',year)
    call codes_get(ibufr,'month',month)
    call codes_get(ibufr,'day',day)
    call codes_get(ibufr,'hour',hour)
    call codes_get(ibufr,'minute',minute)
    call codes_get(ibufr,'second',second,status_time)
    IF (status_time /= CODES_SUCCESS) second = 0.0
    call codes_get(ibufr,'latitude',lat)
    call codes_get(ibufr,'longitude',lon)
    call codes_get(ibufr,'heightOfStationGroundAboveMeanSeaLevel',htg,status_ht)
    IF (status_ht /= CODES_SUCCESS) htg = -999.0
    call codes_get(ibufr,'heightOfBarometerAboveMeanSeaLevel',htp,status_ht)
    IF (status_ht /= CODES_SUCCESS) htp = -999.0
    call codes_get(ibufr,'radiosondeType',sondeType)
    call codes_get(ibufr,'heightOfStation',htec,status_ht) ! Height from WMO list (BUFR)
    IF (status_ht == CODES_SUCCESS .AND. htg == -999.0) htg = htec
    ymd = INT(year)*10000+INT(month)*100+INT(day)
    hms = INT(hour)*10000+INT(minute)*100+INT(second)
    call codes_get(ibufr,'radiosondeSerialNumber',rsnumber,status_rsno)
    call codes_get(ibufr,'softwareVersionNumber',rssoftware,status_rssoft)

    ! Ascent (skip incomplete reports for now)
    call codes_get(ibufr,'timePeriod',timeVal,status_time)
    IF (status_time /= CODES_SUCCESS) THEN
      write(*,'(A,I7,A,I2.2,A,I3.3,I9,I7.6,F9.3,F10.3,2F7.1,I4)') 'Ob: ',count, &
        ' ',blockNumber,' ',stationNumber,ymd,hms,lat(1),lon(1),htg,htp,INT(sondeType)
      write(*,'(A)') 'Missing times - skip'
      llskip=.True.
    ENDIF
    call codes_get(ibufr,'pressure',presVal,status_p)
    IF (status_p /= CODES_SUCCESS) THEN
      write(*,'(A,I7,A,I2.2,A,I3.3,I9,I7.6,F9.3,F10.3,2F7.1,I4)') 'Ob: ',count, &
        ' ',blockNumber,' ',stationNumber,ymd,hms,lat(1),lon(1),htg,htp,INT(sondeType)
      write(*,'(A)') 'Missing pressures - skip'
      llskip=.True.
    ENDIF
    call codes_get(ibufr,'nonCoordinateGeopotentialHeight',zVal,status_ht)
    IF (status_ht /= CODES_SUCCESS) THEN
      write(*,'(A,I7,A,I2.2,A,I3.3,I9,I7.6,F9.3,F10.3,2F7.1,I4)') 'Ob: ',count, &
        ' ',blockNumber,' ',stationNumber,ymd,hms,lat(1),lon(1),htg,htp,INT(sondeType)
      write(*,'(A)') 'Missing heights - skip'
      llskip=.True.
    ENDIF
    ! IF (blockNumber /= 17 .OR. stationNumber /= 196) llskip=.True.  ! FIX
    ! IF (blockNumber /= 17.0) llskip=.True.  ! FIX

    IF (.NOT.llskip) THEN
      call codes_get(ibufr,'latitudeDisplacement',dlatVal)
      call codes_get(ibufr,'longitudeDisplacement',dlonVal)
      call codes_get(ibufr,'extendedVerticalSoundingSignificance',vssVal)
      !call codes_get(ibufr,'geopotentialHeight',zVal)
      call codes_get(ibufr,'airTemperature',tVal)
      call codes_get(ibufr,'dewpointTemperature',tdVal)
      call codes_get(ibufr,'windDirection',wdirVal)
      call codes_get(ibufr,'windSpeed',wspVal)

      ! ---- Array sizes (pressure size can be larger - wind shear levels)
      sizews = size(wspVal)

      ! ---- Print the values --------------------------------
      write(*,'(A,A72)') 'Statid: ',statid
      write(*,'(A,I7,A,I2.2,A,I3.3,I9,I7.6,F9.3,F10.3,2F7.1,I4,I5)') 'Ob: ',count, &
          ' ',blockNumber,' ',stationNumber,ymd,hms,lat(1),lon(1),htg,htp,INT(sondeType),sizews
      IF (status_ht == CODES_SUCCESS) write(*,'(A,F9.3,F10.3,F7.1)')  &
         'WMO list lat, lon, ht: ', lat(1),lon(1),htec
      IF (status_rsno == CODES_SUCCESS) write(*,'(A,A,A)')  &
         'Radiosonde number/software: ', rsnumber, rssoftware
      write(*,'(A)') 'level  dtime   dlat   dlon pressure geopotH airTemp  dewPtT windDir  windSp  signif'
      do i=1,sizews
    Note = '        '
    iflag = vssVal(i)
    IF (i > 1) THEN
          IF (presVal(i) > presVal(i-1) .OR. zVal(i) < zVal(i-1) &
          .OR. timeVal(i) < timeVal(i-1)) Note=' OOO    '
          IF ((timeVal(i)-timeVal(i-1))>120) Note=' tjump  '
          IF (ABS(dlatVal(i)-dlatVal(i-1))>0.1 .OR. &
          ABS(dlonVal(i)-dlonVal(i-1))>0.1) THEN
        Note=' pjump  '
        IF (dlatVal(i) == CODES_MISSING_DOUBLE .OR. &
        dlatVal(i-1) == CODES_MISSING_DOUBLE) Note=' pmiss  '
          ENDIF
    ENDIF
    IF (.NOT.llstdonly .OR. BTEST(iflag,16)) &
    write(*,'(I5,F7.1,2F7.3,F9.1,F8.1,4F8.2,I8,A)') i,timeVal(i), &
          dlatVal(i),dlonVal(i),presVal(i),zVal(i),tVal(i),tVal(i)-tdVal(i), &
      wdirVal(i),wspVal(i),INT(vssVal(i)), Note
      end do

      ! free arrays
      deallocate(dlatVal,dlonVal,vssVal)
      deallocate(presVal,zVal,tVal,tdVal,wdirVal,wspVal)
    END IF
    IF (ALLOCATED(timeVal)) deallocate(timeVal)

    ! release the bufr message
    call codes_release(ibufr)

    ! load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)

  end do

  ! close file
  call codes_close_file(ifile)

end program bufr_read_temp
