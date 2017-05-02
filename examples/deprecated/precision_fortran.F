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
C  Fortran 77 Implementation: precision
C
C  Description: how to control decimal precision when packing fields.
C
C
C
C
C
      program precision
      implicit none
      integer maxNumberOfValues
      parameter (maxNumberOfValues=10000)
      include 'grib_api_fortran.h'
      integer*4 size
      integer infile,outfile
      integer igrib
      real*8 values1(maxNumberOfValues)
      real*8 values2(maxNumberOfValues)
      real*8 maxa,a,maxv,minv,maxr,r
      integer*4 decimalPrecision,bitsPerValue1,bitsPerValue2
      integer i

      call grib_check(grib_open_file(infile
     X,'../data/regular_latlon_surface.grib1','r'))

      call grib_check(grib_open_file(outfile
     X,'../data/regular_latlon_surface_prec.grib1','w'))

C     a new grib message is loaded from file
C     igrib is the grib id to be used in subsequent calls
      call grib_check(grib_new_from_file(infile,igrib))

C     bitsPerValue before changing the packing parameters
      call grib_check(grib_get_int(igrib,'bitsPerValue',bitsPerValue1))

C     get the size of the values array
      call grib_check(grib_get_size(igrib,"values",size))

C     get data values before changing the packing parameters*/
      call grib_check(grib_get_real8_array(igrib,"values",values1,size))

C     setting decimal precision=2 means that 2 decimal digits
C     are preserved when packing.
      decimalPrecision=2
      call grib_check(grib_set_int(igrib,"setDecimalPrecision"
     X,decimalPrecision))

C     bitsPerValue after changing the packing parameters
      call grib_check(grib_get_int(igrib,"bitsPerValue",bitsPerValue2))

C     get data values after changing the packing parameters
      call grib_check(grib_get_real8_array(igrib,"values",values2,size))

C     computing error
      maxa=0
      maxr=0
      maxv=values2(1)
      minv=maxv
      do i=1,size
        a=abs(values2(i)-values1(i))
        if ( values2(i) .gt. maxv ) maxv=values2(i)
        if ( values2(i) .lt. maxv ) minv=values2(i)
        if ( values2(i) .ne. 0 ) then
         r=abs((values2(i)-values1(i))/values2(i))
        endif
        if ( a .gt. maxa ) maxa=a
        if ( r .gt. maxr ) maxr=r
      enddo
      write(*,*) "max absolute error = ",maxa
      write(*,*) "max relative error = ",maxr
      write(*,*) "min value = ",minv
      write(*,*) "max value = ",maxv

      write(*,*) "old number of bits per value=",bitsPerValue1
      write(*,*) "new number of bits per value=",bitsPerValue2

C     write modified message to a file
      call grib_check(grib_write(igrib,outfile))

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(infile))

      call grib_check(grib_close_file(outfile))

      end

