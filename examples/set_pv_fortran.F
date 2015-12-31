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
C  Fortran 77 Implementation: set_fortran
C
C  Description: how to set pv  values.
C
C
C
C
C
      program set
      implicit none
      include 'grib_api_fortran.h'
      integer err
      integer infile,outfile
      integer igrib,iret
      character*256 error
      real*8 pv
      integer pvsize
      dimension pv(4)

      pvsize=4
      infile=5
      outfile=6

      call grib_check(grib_open_file(infile
     X,'../data/regular_latlon_surface.grib1','r'))

      call grib_check(grib_open_file(outfile,'out.grib1','w'))

C     a new grib message is loaded from file
C     igrib is the grib id to be used in subsequent calls
      call grib_check(grib_new_from_file(infile,igrib))

      call grib_check(grib_set_int(igrib,'PVPresent',1))
      call grib_check(grib_set_real8_array(igrib,'pv',pv,pvsize))

C     write modified message to a file
      call grib_check(grib_write(igrib,outfile))

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(infile))

      call grib_check(grib_close_file(outfile))

      end
