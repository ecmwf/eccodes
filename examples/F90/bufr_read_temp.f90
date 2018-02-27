! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 Implementation: bufr_read_temp
!
! Description: how to read levels from TEMP BUFR messages.
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
  integer(kind=4),dimension(:), allocatable  :: timePeriod,extendedVerticalSoundingSignificance
  integer(kind=4)  :: blockNumber,stationNumber
  real(kind=8),dimension(:), allocatable :: pressure,airTemperature,dewpointTemperature
  real(kind=8),dimension(:), allocatable :: geopotentialHeight,latitudeDisplacement,longitudeDisplacement
  real(kind=8),dimension(:), allocatable :: windDirection,windSpeed

  call codes_open_file(ifile,'../../data/bufr/PraticaTemp.bufr','r')

  ! The first bufr message is loaded from file,
  ! ibufr is the bufr id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)
  do while (iret/=CODES_END_OF_FILE)
    write(*,*) 'message: ',count
    call codes_set(ibufr,'unpack',1)
    call codes_get(ibufr,'timePeriod',timePeriod)
    call codes_get(ibufr,'pressure',pressure)
    call codes_get(ibufr,'extendedVerticalSoundingSignificance',extendedVerticalSoundingSignificance)
    call codes_get(ibufr,'nonCoordinateGeopotentialHeight',geopotentialHeight)
    call codes_get(ibufr,'latitudeDisplacement',latitudeDisplacement)
    call codes_get(ibufr,'longitudeDisplacement',longitudeDisplacement)
    call codes_get(ibufr,'airTemperature',airTemperature)
    call codes_get(ibufr,'dewpointTemperature',dewpointTemperature)
    call codes_get(ibufr,'windDirection',windDirection)
    call codes_get(ibufr,'windSpeed',windSpeed)
    call codes_get(ibufr,'blockNumber',blockNumber)
    call codes_get(ibufr,'stationNumber',stationNumber)
    print *,'station',blockNumber,stationNumber
    print *,'timePeriod pressure geopotentialHeight latitudeDisplacement &
          &longitudeDisplacement airTemperature windDirection windSpeed significance'
    do i=1,size(windSpeed)
      write(*,'(I5,6X,F9.1,2X,F9.2,10X,F8.2,14X,F8.2,16X,F8.2,6X,F8.2,4X,F8.2,4X,I0)') timePeriod(i),pressure(i),&
          &geopotentialHeight(i),latitudeDisplacement(i),&
          &longitudeDisplacement(i),airTemperature(i),windDirection(i),windSpeed(i),extendedVerticalSoundingSignificance(i)
    enddo
    ! Free arrays
    deallocate(timePeriod)
    deallocate(pressure)
    deallocate(geopotentialHeight)
    deallocate(latitudeDisplacement)
    deallocate(longitudeDisplacement)
    deallocate(airTemperature)
    deallocate(dewpointTemperature)
    deallocate(windDirection)
    deallocate(windSpeed)
    deallocate(extendedVerticalSoundingSignificance)
    ! Release the bufr message
    call codes_release(ibufr)
    ! Load the next bufr message
    call codes_bufr_new_from_file(ifile,ibufr,iret)
    count=count+1
  end do
  ! Close file
  call codes_close_file(ifile)

end program bufr_read_temp
