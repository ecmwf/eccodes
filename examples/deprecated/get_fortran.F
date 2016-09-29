C Copyright 2005-2016 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C
C  Fortran 77  Implementation: get_fortran
C
C  Description: how to get values using keys.
C
C
C
C
      program get
      implicit none
      integer maxNumberOfValues
      parameter( maxNumberOfValues = 10000 )
      include 'grib_api_fortran.h'
      integer ifile
      integer iret
      integer igrib
      integer i
      real*8 latitudeOfFirstPointInDegrees
      real*8 longitudeOfFirstPointInDegrees
      real*8 latitudeOfLastPointInDegrees
      real*8 longitudeOfLastPointInDegrees
      real*8 jDirectionIncrementInDegrees
      real*8 iDirectionIncrementInDegrees
      integer*4 numberOfPointsAlongAParallel
      integer*4 numberOfPointsAlongAMeridian
      real*8 values(maxNumberOfValues)
      integer*4 numberOfValues
      real*8 average
      character*256 error
      integer*4 size

      size=maxNumberOfValues
      ifile=5

      iret=grib_open_file(ifile
     X,'../data/reduced_latlon_surface.grib1','r')
      call grib_check(iret)

C     a new grib message is loaded from file
C     igrib is the grib id to be used in subsequent calls
      call grib_check( grib_new_from_file(ifile,igrib) )

C     get as a integer
      call grib_check(grib_get_int(igrib,'numberOfPointsAlongAParallel'
     X,numberOfPointsAlongAParallel) )
      write(*,*) 'numberOfPointsAlongAParallel='
     X,numberOfPointsAlongAParallel

C     get as a integer
      call grib_check( grib_get_int(igrib,'numberOfPointsAlongAMeridian'
     X,numberOfPointsAlongAMeridian) )
      write(*,*) 'numberOfPointsAlongAMeridian='
     X,numberOfPointsAlongAMeridian

C     get as a real8
      call grib_check( grib_get_real8(igrib
     X,'latitudeOfFirstGridPointInDegrees'
     X,latitudeOfFirstPointInDegrees) )
       write(*,*) 'latitudeOfFirstGridPointInDegrees='
     X,latitudeOfFirstPointInDegrees

C     get as a real8
      call grib_check( grib_get_real8(igrib
     X,'longitudeOfFirstGridPointInDegrees'
     X,longitudeOfFirstPointInDegrees) )
       write(*,*) 'longitudeOfFirstGridPointInDegrees='
     X,longitudeOfFirstPointInDegrees

C     get as a real8
      call grib_check( grib_get_real8(igrib
     X,'latitudeOfLastGridPointInDegrees'
     X,latitudeOfLastPointInDegrees) )
       write(*,*) 'latitudeOfLastGridPointInDegrees='
     X,latitudeOfLastPointInDegrees

C     get as a real8
      call grib_check( grib_get_real8(igrib
     X,'longitudeOfLastGridPointInDegrees'
     X,longitudeOfLastPointInDegrees) )
      write(*,*) 'longitudeOfLastGridPointInDegrees='
     X,longitudeOfLastPointInDegrees

C     get as a real8
      call grib_check( grib_get_real8(igrib
     X,'jDirectionIncrementInDegrees'
     X,jDirectionIncrementInDegrees) )
      write(*,*) 'jDirectionIncrementInDegrees='
     X,jDirectionIncrementInDegrees

C     get as a real8
      call grib_check( grib_get_real8(igrib
     X,'iDirectionIncrementInDegrees'
     X,iDirectionIncrementInDegrees) )
      write(*,*) 'iDirectionIncrementInDegrees='
     X,iDirectionIncrementInDegrees

C     get the size of the values array
      call grib_check(grib_get_size(igrib,'values',numberOfValues))
      write(*,*) 'numberOfValues=',numberOfValues

C     get data values
      call grib_check(grib_get_real8_array(igrib,'values',values,size))
      if ( size .ne. numberOfValues ) then
        write(*,*) 'ERROR: wrong numberOfValues'
        stop
      endif

      average = 0
      do i=1,numberOfValues
        average = average + values(i);
      enddo

      average =average / numberOfValues

      write(*,*)'There are ',numberOfValues
     X,' average is ',average

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(ifile))

      end
