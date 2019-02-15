! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: how to get values using keys.
!
!
program get
use eccodes
implicit none

  integer                                         ::  ifile
  integer                                         ::  iret
  integer                                         ::  igrib
  integer                                         ::  i
  real(kind=8)                                    ::  latitudeOfFirstPointInDegrees
  real(kind=8)                                    ::  longitudeOfFirstPointInDegrees
  real(kind=8)                                    ::  latitudeOfLastPointInDegrees
  real(kind=8)                                    ::  longitudeOfLastPointInDegrees
  real(kind=8)                                    ::  jDirectionIncrementInDegrees
  real(kind=8)                                    ::  iDirectionIncrementInDegrees
  integer(kind = 4)                               ::  numberOfPointsAlongAParallel
  integer(kind = 4)                               ::  numberOfPointsAlongAMeridian
  real(kind=8), dimension(:), allocatable         ::  values
  integer(kind = 4)                               ::  numberOfValues
  real(kind=8)                                    ::  average

  call codes_open_file(ifile, &
       '../../data/reduced_latlon_surface.grib1','r')

  ! A new grib message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call  codes_grib_new_from_file(ifile,igrib)

  ! get as a integer
  call codes_get(igrib,'numberOfPointsAlongAParallel', &
                               numberOfPointsAlongAParallel)
  write(*,*) 'numberOfPointsAlongAParallel=', &
              numberOfPointsAlongAParallel

  ! get as a integer
  call codes_get(igrib,'numberOfPointsAlongAMeridian', &
                                       numberOfPointsAlongAMeridian)
  write(*,*) 'numberOfPointsAlongAMeridian=', &
              numberOfPointsAlongAMeridian

  ! get as a real8
  call codes_get(igrib, &
                      'latitudeOfFirstGridPointInDegrees', &
                       latitudeOfFirstPointInDegrees)
  write(*,*) 'latitudeOfFirstGridPointInDegrees=', &
            latitudeOfFirstPointInDegrees

  !     get as a real8
  call codes_get(igrib, &
               'longitudeOfFirstGridPointInDegrees', &
                longitudeOfFirstPointInDegrees)
  write(*,*) 'longitudeOfFirstGridPointInDegrees=', &
      longitudeOfFirstPointInDegrees

  ! get as a real8
  call codes_get(igrib, &
      'latitudeOfLastGridPointInDegrees', &
      latitudeOfLastPointInDegrees)
  write(*,*) 'latitudeOfLastGridPointInDegrees=', &
      latitudeOfLastPointInDegrees

  ! get as a real8
  call codes_get(igrib, &
      'longitudeOfLastGridPointInDegrees', &
      longitudeOfLastPointInDegrees)
  write(*,*) 'longitudeOfLastGridPointInDegrees=', &
              longitudeOfLastPointInDegrees

  ! get as a real8
  call codes_get(igrib, &
                   'jDirectionIncrementInDegrees', &
                    jDirectionIncrementInDegrees)
  write(*,*) 'jDirectionIncrementInDegrees=', &
              jDirectionIncrementInDegrees

  ! get as a real8
  call codes_get(igrib, &
       'iDirectionIncrementInDegrees', &
        iDirectionIncrementInDegrees)
  write(*,*) 'iDirectionIncrementInDegrees=', &
              iDirectionIncrementInDegrees

  ! get the size of the values array
  call codes_get_size(igrib,'values',numberOfValues)
  write(*,*) 'numberOfValues=',numberOfValues

  allocate(values(2*numberOfValues), stat=iret)
  ! get data values
  print*, size(values)
  call codes_get(igrib,'values',values)

  average = 0
  do i=1,numberOfValues
        average = average + values(i);
  enddo

  average =average / numberOfValues

  write(*,*)'There are ',numberOfValues, &
            ' average is ',average

  call codes_release(igrib)

  call codes_close_file(ifile)

  deallocate(values)
end program get
