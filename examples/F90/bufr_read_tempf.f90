! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! Description: read and print radiosonde data from TEMP BUFR messages.
! If available this version also lists the position information from the WMO list
! (now OSCAR/Surface) appended to the reports by ECMWF
!
! Author: Bruce Ingleby
!
! Please note that TEMP reports can be encoded in various ways in BUFR. Therefore the code
! below might not work directly for other types of TEMP messages than the one used in the
! example. It is advised to use bufr_dump first to understand the structure of these messages.
!
program bufr_read_tempf
  use eccodes
  implicit none
  integer            :: ifile
  integer            :: iret
  integer            :: ibufr
  integer            :: i, count = 0
  integer            :: iflag
  integer            :: status_id, status_ht, status_time = 0, status_p
  integer            :: status_airt, status_dewt
  integer            :: status_rsno, status_rssoft, status_balloonwt
  integer(kind=4)    :: sizews
  integer(kind=4)    :: blockNumber, stationNumber
  integer(kind=4)    :: ymd, hms
  logical            :: llstdonly = .True.  ! Set True to list standard levels only
  logical            :: llskip
  real(kind=8)       :: year, month, day, hour, minute, second
  real(kind=8)       :: htg, htp, htec = 0, sondeType, balloonwt
  ! real(kind=8), dimension(:), allocatable :: descriptors
  real(kind=8), dimension(:), allocatable :: lat, lon
  real(kind=8), dimension(:), allocatable :: timeVal, dlatVal, dlonVal, vssVal
  real(kind=8), dimension(:), allocatable :: presVal, zVal, tVal, tdVal, wdirVal, wspVal
  character(len=128) :: statid, dropid
  character(len=16)  :: rsnumber
  character(len=16)  :: rssoftware

  call codes_open_file(ifile, '../../data/bufr/PraticaTemp.bufr', 'r')

  ! the first BUFR message is loaded from file
  ! ibufr is the BUFR id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile, ibufr, iret)

  ! loop through all messages in the file
  do while (iret /= CODES_END_OF_FILE)

    ! Can check the template used
    ! call codes_get(ibufr,'unexpandedDescriptors',descriptors)
    ! write(0,*) 'Template: ', descriptors
    ! IF (descriptors(1) /= 309056.0) GOTO 999 ! only list descent profiles

    ! we need to instruct ecCodes to expand all the descriptors
    ! i.e. unpack the data values
    call codes_set(ibufr, "unpack", 1);
    ! In our BUFR message verticalSoundingSignificance is always followed by
    !      geopotential, airTemperature, dewpointTemperature,
    !      windDirection, windSpeed and pressure.

    count = count + 1
    llskip = .False.

    ! Metadata:
    call codes_get(ibufr, 'aircraftRegistrationNumberOrOtherIdentification', dropid, status_id)
    IF (status_id /= CODES_SUCCESS) dropid = "UNKNOWN "
    call codes_get(ibufr, 'shipOrMobileLandStationIdentifier', statid, status_id)
    IF (status_id /= CODES_SUCCESS) statid = dropid
    ! call codes_is_missing(ibufr, 'shipOrMobileLandStationIdentifier', statid_missing)
    ! IF (statid_missing == 1) statid = "MISSING"
    call codes_get(ibufr, 'blockNumber', blockNumber)
    call codes_get(ibufr, 'stationNumber', stationNumber)
    IF (blockNumber <= 99.0 .AND. stationNumber <= 1000) write (statid, '(I2.2,I3.3,3X)') blockNumber, stationNumber

    call codes_get(ibufr, 'year', year)
    call codes_get(ibufr, 'month', month)
    call codes_get(ibufr, 'day', day)
    call codes_get(ibufr, 'hour', hour)
    call codes_get(ibufr, 'minute', minute)
    call codes_get(ibufr, 'second', second, status_time)
    IF (status_time /= CODES_SUCCESS) second = 0.0
    call codes_get(ibufr, 'latitude', lat)
    call codes_get(ibufr, 'longitude', lon)
    call codes_get(ibufr, 'heightOfStationGroundAboveMeanSeaLevel', htg, status_ht)
    IF (status_ht /= CODES_SUCCESS) htg = -999.0
    call codes_get(ibufr, 'heightOfBarometerAboveMeanSeaLevel', htp, status_ht)
    IF (status_ht /= CODES_SUCCESS) htp = -999.0
    call codes_get(ibufr, 'radiosondeType', sondeType)
    call codes_get(ibufr, 'heightOfStation', htec, status_ht) ! Height from WMO list (BUFR)
    IF (status_ht == CODES_SUCCESS .AND. htg == -999.0) htg = htec
    ymd = INT(year)*10000 + INT(month)*100 + INT(day)
    hms = INT(hour)*10000 + INT(minute)*100 + INT(second)
    call codes_get(ibufr, 'radiosondeSerialNumber', rsnumber, status_rsno)
    call codes_get(ibufr, 'softwareVersionNumber', rssoftware, status_rssoft)
    call codes_get(ibufr, 'weightOfBalloon', balloonwt, status_balloonwt)
    IF (status_balloonwt /= CODES_SUCCESS) balloonwt = 0.0

    ! Ascent (skip reports without dtime array for now)
    call codes_get(ibufr, 'timePeriod', timeVal, status_time)
    IF (status_time /= CODES_SUCCESS) THEN
      write (*, '(A,I7,A,A8,I9,I7.6,F9.3,F10.3,2F7.1,I4)') 'Ob: ', count, &
        ' ', statid, ymd, hms, lat(1), lon(1), htg, htp, INT(sondeType)
      write (*, '(A)') 'Missing times - skip'
      llskip = .True.
    END IF
    call codes_get(ibufr, 'pressure', presVal, status_p)
    call codes_get(ibufr, 'nonCoordinateGeopotentialHeight', zVal, status_ht)

    IF (.NOT. llskip) THEN
      call codes_get(ibufr, 'latitudeDisplacement', dlatVal)
      call codes_get(ibufr, 'longitudeDisplacement', dlonVal)
      call codes_get(ibufr, 'extendedVerticalSoundingSignificance', vssVal)
      call codes_get(ibufr, 'airTemperature', tVal, status_airt)
      call codes_get(ibufr, 'dewpointTemperature', tdVal, status_dewt)
      call codes_get(ibufr, 'windDirection', wdirVal)
      call codes_get(ibufr, 'windSpeed', wspVal)

      ! ---- Array sizes (pressure size can be larger - wind shear levels)
      sizews = size(wspVal)

      IF (status_p /= CODES_SUCCESS) THEN
        allocate(presVal(sizews))
        presVal(:) = -999999999.0
      END IF
      IF (status_ht /= CODES_SUCCESS) THEN
        allocate(zVal(sizews))
        zVal(:) = -999999999.0
      END IF
      IF (status_airt /= CODES_SUCCESS) THEN
        allocate(tVal(sizews))
        tVal(:) = -999999999.0
      END IF
      IF (status_dewt /= CODES_SUCCESS) THEN
        allocate(tdVal(sizews))
        tdVal(:) = -999999999.0
      END IF

      ! ---- Print the values --------------------------------
      write (*, '(A,I7,A,A8,I9,I7.6,F9.3,F10.3,2F7.1,I4,I5)') 'Ob: ', count, &
        ' ', statid, ymd, hms, lat(1), lon(1), htg, htp, INT(sondeType), sizews
      IF (status_rsno == CODES_SUCCESS) write (*, '(A,A,A,F7.3)') &
        'RS number/software/balloonwt: ', rsnumber, rssoftware, balloonwt
      IF (status_ht == CODES_SUCCESS .AND. SIZE(lat) > 1) write (*, '(A,A,F9.3,F10.3,F7.1)') &
        'WMO list lat, lon, ht: ', statid, lat(2), lon(2), htec
      write (*, '(A)') 'level  dtime   dlat   dlon pressure geopotH airTemp  dewPtT windDir  windSp  signif'
      do i = 1, sizews
        iflag = vssVal(i)
        IF (.NOT. llstdonly .OR. BTEST(iflag, 16)) &
          write (*, '(I5,F7.1,2F7.3,F9.1,F8.1,4F8.2,I8)') i, timeVal(i), &
          dlatVal(i), dlonVal(i), presVal(i), zVal(i), tVal(i), tdVal(i), &
          wdirVal(i), wspVal(i), INT(vssVal(i))
      end do

    END IF
    ! free allocated arrays
    IF (ALLOCATED(timeVal)) deallocate (timeVal)
    IF (ALLOCATED(dlatVal)) deallocate (dlatVal)
    IF (ALLOCATED(dlonVal)) deallocate (dlonVal)
    IF (ALLOCATED(vssVal)) deallocate (vssVal)
    IF (ALLOCATED(presVal)) deallocate (presVal)
    IF (ALLOCATED(zVal)) deallocate (zVal)
    IF (ALLOCATED(tVal)) deallocate (tVal)
    IF (ALLOCATED(tdVal)) deallocate (tdVal)
    IF (ALLOCATED(wdirVal)) deallocate (wdirVal)
    IF (ALLOCATED(wspVal)) deallocate (wspVal)
    IF (ALLOCATED(lat)) deallocate (lat)
    IF (ALLOCATED(lon)) deallocate (lon)

    ! release the BUFR message
    call codes_release(ibufr)

    ! load the next BUFR message
    call codes_bufr_new_from_file(ifile, ibufr, iret)

  end do

  call codes_close_file(ifile)
  print*, 'Finishing normally. Number of BUFR records read: ', count

end program bufr_read_tempf
