! Copyright 2005-2015 ECMWF.
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
  integer                            ::  is_missing, is_defined
  
  call codes_open_file(ifile, &
       '../../data/reduced_latlon_surface.grib1','r')
  
  ! Loop on all the messages in a file.
  
  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call  codes_grib_new_from_file(ifile,igrib, iret) 
  
  LOOP: DO WHILE (iret /= CODES_END_OF_FILE)

     !check if the value of the key is MISSING
     is_missing=0;
     call codes_is_missing(igrib,'Ni',is_missing);
     if ( is_missing /= 1 ) then
        !     get as a integer
        call codes_get(igrib,'Ni',numberOfPointsAlongAParallel) 
        write(*,*) 'numberOfPointsAlongAParallel=', &
             numberOfPointsAlongAParallel
     else
        write(*,*) 'numberOfPointsAlongAParallel is missing'
     endif     
     
     !check for existence of keys
     is_defined=0;
     call codes_is_defined(igrib,'edition',is_defined);
     if ( is_defined == 0 ) then
         write(0,*) 'ERROR: An expected key was not defined!!'
         call exit(1)
     endif
     call codes_is_defined(igrib,'ThisIsNoLoveSong',is_defined);
     if ( is_defined == 1 ) then
         write(0,*) 'ERROR: An unexpected key was defined!!'
         call exit(1)
     endif

     !     get as a integer
     call codes_get(igrib,'Nj',numberOfPointsAlongAMeridian) 
     write(*,*) 'numberOfPointsAlongAMeridian=', &
          numberOfPointsAlongAMeridian
     
     !     get as a real
     call codes_get(igrib, 'latitudeOfFirstGridPointInDegrees', &
          latitudeOfFirstPointInDegrees) 
     write(*,*) 'latitudeOfFirstGridPointInDegrees=', &
          latitudeOfFirstPointInDegrees
     
     !     get as a real
     call codes_get(igrib, 'longitudeOfFirstGridPointInDegrees', &
          longitudeOfFirstPointInDegrees) 
     write(*,*) 'longitudeOfFirstGridPointInDegrees=', &
          longitudeOfFirstPointInDegrees
     
     !     get as a real
     call codes_get(igrib, 'latitudeOfLastGridPointInDegrees', &
          latitudeOfLastPointInDegrees) 
     write(*,*) 'latitudeOfLastGridPointInDegrees=', &
          latitudeOfLastPointInDegrees
     
     !     get as a real
     call codes_get(igrib, 'longitudeOfLastGridPointInDegrees', &
          longitudeOfLastPointInDegrees) 
     write(*,*) 'longitudeOfLastGridPointInDegrees=', &
          longitudeOfLastPointInDegrees
     
     
     !     get the size of the values array
     call codes_get_size(igrib,'values',numberOfValues)
     write(*,*) 'numberOfValues=',numberOfValues
     
     allocate(values(numberOfValues), stat=iret)
     !     get data values
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
