!
!  Description: how to get values using keys.
!
!
!  Copyright: See COPYING file that comes with this distribution
!
!
program get
use grib_api
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
  character(len = 256)                            ::  error

  call grib_open_file(ifile, &
       '../../data/reduced_latlon_surface.grib1','r')
 
!     a new grib message is loaded from file
!     igrib is the grib id to be used in subsequent calls
  call  grib_new_from_file(ifile,igrib) 

!     get as a integer
  call grib_get(igrib,'numberOfPointsAlongAParallel', &
                               numberOfPointsAlongAParallel) 
  write(*,*) 'numberOfPointsAlongAParallel=', &
              numberOfPointsAlongAParallel

!     get as a integer
  call grib_get(igrib,'numberOfPointsAlongAMeridian', &
                                       numberOfPointsAlongAMeridian) 
  write(*,*) 'numberOfPointsAlongAMeridian=', &
              numberOfPointsAlongAMeridian

!     get as a real8
call grib_get(igrib, &
                      'latitudeOfFirstGridPointInDegrees', &
                       latitudeOfFirstPointInDegrees) 
write(*,*) 'latitudeOfFirstGridPointInDegrees=', &
            latitudeOfFirstPointInDegrees

!     get as a real8
  call grib_get(igrib, &
               'longitudeOfFirstGridPointInDegrees', &
                longitudeOfFirstPointInDegrees) 
  write(*,*) 'longitudeOfFirstGridPointInDegrees=', &
     longitudeOfFirstPointInDegrees

!     get as a real8
  call grib_get(igrib, &
     'latitudeOfLastGridPointInDegrees', &
     latitudeOfLastPointInDegrees) 
  write(*,*) 'latitudeOfLastGridPointInDegrees=', &
     latitudeOfLastPointInDegrees

!     get as a real8
  call grib_get(igrib, &
     'longitudeOfLastGridPointInDegrees', &
      longitudeOfLastPointInDegrees) 
  write(*,*) 'longitudeOfLastGridPointInDegrees=', &
              longitudeOfLastPointInDegrees

!     get as a real8
  call grib_get(igrib, &
                   'jDirectionIncrementInDegrees', &
                    jDirectionIncrementInDegrees) 
  write(*,*) 'jDirectionIncrementInDegrees=', &
              jDirectionIncrementInDegrees

!     get as a real8
  call grib_get(igrib, &
       'iDirectionIncrementInDegrees', &
        iDirectionIncrementInDegrees) 
  write(*,*) 'iDirectionIncrementInDegrees=', &
              iDirectionIncrementInDegrees

!     get the size of the values array
  call grib_get_size(igrib,'values',numberOfValues)
  write(*,*) 'numberOfValues=',numberOfValues

  allocate(values(2*numberOfValues), stat=iret)
!     get data values
  print*, size(values)
  call grib_get(igrib,'values',values)


  average = 0
  do i=1,numberOfValues
        average = average + values(i);
  enddo

  average =average / numberOfValues

  write(*,*)'There are ',numberOfValues, &
            ' average is ',average

  call grib_release(igrib)

  call grib_close_file(ifile)

  deallocate(values)
end program get
