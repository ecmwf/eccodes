C Copyright 2005-2017 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C
C  Fortran 77  Implementation: print_data_fortran
C
C  Description: prints all the data contained in a grib file
C
C
C
C
      program print_data_fortran
      implicit none
      integer maxNumberOfValues
      parameter( maxNumberOfValues = 100000 )
      include 'grib_api_fortran.h'
      integer ifile
      integer iret
      integer igrib
      integer i
      real*8 values(maxNumberOfValues)
      integer*4 numberOfValues
      real*8 average
      real*8 max
      real*8 min
      character*256 error
      integer*4 size

      size=maxNumberOfValues
      ifile=5

      iret=grib_open_file(ifile
     X,'../data/constant_field.grib1','r')
      call grib_check(iret)

C     a new grib message is loaded from file
C     igrib is the grib id to be used in subsequent calls
      call grib_check( grib_new_from_file(ifile,igrib) )


C     get the size of the values array
      call grib_check(grib_get_size(igrib,'values',numberOfValues))
      if ( numberOfValues .gt. maxNumberOfValues ) then
        write(*,*)'ERROR: maxNumberOfValues too small numberOfValues=',
     XnumberOfValues
	    stop
      endif

C     get data values
      call grib_check(grib_get_real8_array(igrib,'values',values,size))
      if ( size .ne. numberOfValues ) then
        write(*,*) 'ERROR: wrong numberOfValues'
        stop
      endif

      do i=1,numberOfValues
        write(*,*)'  ',i,values(i)
      enddo

      average =average / numberOfValues

      write(*,*)numberOfValues,' values found '

      call grib_check(grib_get_real8(igrib,'max',max))
      write(*,*) 'max=',max
      call grib_check(grib_get_real8(igrib,'min',min))
      write(*,*) 'min=',min
      call grib_check(grib_get_real8(igrib,'average',average))
      write(*,*) 'average=',average

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(ifile))

      end
