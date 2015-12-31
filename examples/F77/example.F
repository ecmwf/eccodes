C Copyright 2005-2016 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C

 C
C  Fortran 77 Implementation: keys_iterator
C
C  Description:
C  Example on how to use keys_iterator functions and the
C  grib_keys_iterator structure to get all the available
C  keys in a message.
C
C
C
C
      program keys_iterator
      implicit none
      include 'grib_api_f77.h'
      integer ifile,igrib,iret
      integer grib_count
      integer maxNumberOfValues
      parameter( maxNumberOfValues = 10000 )
      include 'grib_api_fortran.h'
      integer ifile
      integer iret
      integer igrib
      integer i
      real*8 values(maxNumberOfValues)
      integer*4 numberOfValues
      character*256 error
      integer*4 size

      size=maxNumberOfValues
      ifile=5

      iret=grib_open_file(ifile
     X,'../x.grib1','r')

      call grib_check(grib_open_file(outfile
     X,'../out.grib1','w'))

C     Loop on all the messages in a file.
  10  iret=grib_new_from_file(ifile,igrib)
      if (igrib .eq. -1 )  then
        if (iret .ne.0) then
           call grib_check(iret)
        endif
        stop
      endif

      grib_count=grib_count+1
      write(*,*) '-- GRIB N. ',grib_count,' --'

C     a new grib message is loaded from file
C     igrib is the grib id to be used in subsequent calls
      call grib_check( grib_new_from_file(ifile,igrib) )

C     get the size of the values array
      call grib_check(grib_get_size(igrib,'values',numberOfValues))
      write(*,*) 'numberOfValues=',numberOfValues

C     get data values
      call grib_check(grib_get_real8_array(igrib,'values',values,size))
      if ( size .ne. numberOfValues ) then
        write(*,*) 'ERROR: wrong numberOfValues'
        stop
      endif

      do i=1,numberOfValues
       values(i)=values(i)*9.8;
      enddo

      call grib_check(grib_set_real8_array(igrib,'values',values,size))

C     write modified message to a file
      call grib_check(grib_write(igrib,outfile))

      goto 10

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(ifile))

      end

