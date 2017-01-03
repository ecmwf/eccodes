C Copyright 2005-2017 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C
C
C  Fortran 77 Implementation: iterator_fortran
C
C  Description: how to use an iterator on lat/lon/values.
C
C
C
C
C
      program iterator
      implicit none
      include 'grib_api_fortran.h'
      integer ifile
      integer iret,iter
      real*8 lat,lon,value,missingValue
      integer n,flags
      character*256 filename
      character*256 error

C     Message identifier.
      integer igrib

      ifile=5

      call grib_check(grib_open_file(ifile,
     X'../data/regular_latlon_surface.grib1','r'))

C     Loop on all the messages in a file.
  10  iret=grib_new_from_file(ifile,igrib)
      if (igrib .eq. -1 )  then
        if (iret .ne.0) then
           call grib_check(iret)
        endif
        stop
      endif

C     get as a real8
      call grib_check(grib_get_real8(igrib
     X,'missingValue',missingValue))
      write(*,*) 'missingValue=',missingValue

C     A new iterator on lat/lon/values is created from the message igrib
      flags = 0
      call grib_check(grib_iterator_new(igrib,iter,flags))

      n = 0
C     Loop on all the lat/lon/values.
  20  iret = grib_iterator_next(iter,lat,lon,value)
      if ( iret .eq. 0 ) goto 30
C     You can now print lat and lon,
      if ( value .eq. missingValue ) then
C     decide what to print if a missing value is found.
        write(*,*) "- ",n," - lat=",lat," lon=",lon," value=missing"
      else
C     or print the value if is not missing.
        write(*,*) " ",n," lat=",lat," lon=",lon," value=",value
      endif

      n=n+1

      goto 20
  30  continue

C     At the end the iterator is deleted to free memory.
      call grib_check(grib_iterator_delete(iter))

      goto 10

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(ifile))

      end
