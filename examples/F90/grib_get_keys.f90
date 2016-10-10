! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to get values using keys from GRIB messages
!
!
program grib_get_keys
  use eccodes
  implicit none
  
  integer                            ::  ifile
  integer                            ::  iret
  integer                            ::  igrib
  real                               ::  latitudeOfFirstPointInDegrees
  real                               ::  longitudeOfFirstPointInDegrees
  real                               ::  latitudeOfLastPointInDegrees
  real                               ::  longitudeOfLastPointInDegrees
  integer                            ::  numberOfPointsAlongAParallel
  integer                            ::  numberOfPointsAlongAMeridian
  real, dimension(:), allocatable    ::  values
  integer                            ::  numberOfValues
  real                               ::  average,min_val, max_val
  integer                            ::  is_missing
  character(len=10)                  ::  open_mode='r'
  
  call codes_open_file(ifile, &
       '../../data/reduced_latlon_surface.grib1', open_mode)
  
  ! Loop on all the messages in a file.

  ! A new GRIB message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call  codes_grib_new_from_file(ifile,igrib, iret) 

  LOOP: DO WHILE (iret /= CODES_END_OF_FILE)

    ! For debugging
    call grib_dump(igrib)

    ! Check if the value of the key is MISSING
    is_missing=0;
    call codes_is_missing(igrib,'Ni',is_missing);
    if ( is_missing /= 1 ) then
        ! Key value is not missing so get as an integer
        call codes_get(igrib,'Ni',numberOfPointsAlongAParallel) 
        write(*,*) 'numberOfPointsAlongAParallel=', &
             numberOfPointsAlongAParallel
    else
        write(*,*) 'numberOfPointsAlongAParallel is missing'
    endif     

    ! Get as an integer
    call codes_get(igrib,'Nj',numberOfPointsAlongAMeridian) 
    write(*,*) 'numberOfPointsAlongAMeridian=', &
         numberOfPointsAlongAMeridian

    ! Get as a real
    call codes_get(igrib, 'latitudeOfFirstGridPointInDegrees', &
          latitudeOfFirstPointInDegrees) 
    write(*,*) 'latitudeOfFirstGridPointInDegrees=', &
          latitudeOfFirstPointInDegrees

    ! Get as a real
    call codes_get(igrib, 'longitudeOfFirstGridPointInDegrees', &
          longitudeOfFirstPointInDegrees) 
    write(*,*) 'longitudeOfFirstGridPointInDegrees=', &
          longitudeOfFirstPointInDegrees

    ! Get as a real
    call codes_get(igrib, 'latitudeOfLastGridPointInDegrees', &
          latitudeOfLastPointInDegrees) 
    write(*,*) 'latitudeOfLastGridPointInDegrees=', &
          latitudeOfLastPointInDegrees

    ! Get as a real
    call codes_get(igrib, 'longitudeOfLastGridPointInDegrees', &
          longitudeOfLastPointInDegrees) 
    write(*,*) 'longitudeOfLastGridPointInDegrees=', &
          longitudeOfLastPointInDegrees

    ! Get the size of the values array
    call codes_get_size(igrib,'values',numberOfValues)
    write(*,*) 'numberOfValues=',numberOfValues

    allocate(values(numberOfValues), stat=iret)
    ! Get data values
    call codes_get(igrib,'values',values)
    call codes_get(igrib,'min',min_val) ! can also be obtained through minval(values)
    call codes_get(igrib,'max',max_val) ! can also be obtained through maxval(values)
    call codes_get(igrib,'average',average) ! can also be obtained through maxval(values)

    deallocate(values)

    write(*,*)'There are ',numberOfValues, &
          ' average is ',average, &
          ' min is ',  min_val, &
          ' max is ',  max_val

    call codes_release(igrib)

    call codes_grib_new_from_file(ifile,igrib, iret)

  end do LOOP

  call codes_close_file(ifile)

end program grib_get_keys
