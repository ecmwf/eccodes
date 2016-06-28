! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: count messages before processing
!
!
program get
  use eccodes
  implicit none

  integer                            ::  ifile
  integer                            ::  iret
  integer                            ::  n
  integer                            ::  i
  integer,dimension(:),allocatable   ::  igrib
  real                               ::  latitudeOfFirstPointInDegrees
  real                               ::  longitudeOfFirstPointInDegrees
  real                               ::  latitudeOfLastPointInDegrees
  real                               ::  longitudeOfLastPointInDegrees
  integer                            ::  numberOfPointsAlongAParallel
  integer                            ::  numberOfPointsAlongAMeridian
  real, dimension(:), allocatable    ::  values
  integer                            ::  numberOfValues
  real                               ::  average,min_val, max_val

  call codes_open_file(ifile, &
       '../../data/tigge_pf_ecmwf.grib2','r')

  ! count the messages in the file
  call codes_count_in_file(ifile,n)
  allocate(igrib(n))
  igrib=-1

  ! Load the messages from the file.
  DO i=1,n
    call codes_grib_new_from_file(ifile,igrib(i), iret)
  END DO

  ! we can close the file
  call codes_close_file(ifile)

  ! Loop on all the messages in memory
  DO i=1,n
    write(*,*) 'processing message number ',i
    !     get as a integer
    call codes_get(igrib(i),'Ni',numberOfPointsAlongAParallel)
    write(*,*) 'numberOfPointsAlongAParallel=', &
          numberOfPointsAlongAParallel

    !     get as a integer
    call codes_get(igrib(i),'Nj',numberOfPointsAlongAMeridian)
    write(*,*) 'numberOfPointsAlongAMeridian=', &
          numberOfPointsAlongAMeridian

    !     get as a real
    call codes_get(igrib(i), 'latitudeOfFirstGridPointInDegrees', &
         latitudeOfFirstPointInDegrees)
    write(*,*) 'latitudeOfFirstGridPointInDegrees=', &
          latitudeOfFirstPointInDegrees

    !     get as a real
    call codes_get(igrib(i), 'longitudeOfFirstGridPointInDegrees', &
         longitudeOfFirstPointInDegrees)
    write(*,*) 'longitudeOfFirstGridPointInDegrees=', &
         longitudeOfFirstPointInDegrees

    !     get as a real
    call codes_get(igrib(i), 'latitudeOfLastGridPointInDegrees', &
         latitudeOfLastPointInDegrees)
    write(*,*) 'latitudeOfLastGridPointInDegrees=', &
         latitudeOfLastPointInDegrees

    !     get as a real
    call codes_get(igrib(i), 'longitudeOfLastGridPointInDegrees', &
         longitudeOfLastPointInDegrees)
    write(*,*) 'longitudeOfLastGridPointInDegrees=', &
         longitudeOfLastPointInDegrees


    !     get the size of the values array
    call codes_get_size(igrib(i),'values',numberOfValues)
    write(*,*) 'numberOfValues=',numberOfValues

    allocate(values(numberOfValues), stat=iret)
    !     get data values
    call codes_get(igrib(i),'values',values)
    call codes_get(igrib(i),'min',min_val) ! can also be obtained through minval(values)
    call codes_get(igrib(i),'max',max_val) ! can also be obtained through maxval(values)
    call codes_get(igrib(i),'average',average) ! can also be obtained through maxval(values)

    write(*,*)'There are ',numberOfValues, &
          ' average is ',average, &
          ' min is ',  min_val, &
          ' max is ',  max_val
    write(*,*) '---------------------'
    deallocate(values)
  END DO

  DO i=1,n
    call codes_release(igrib(i))
  END DO

  deallocate(igrib)

end program get
