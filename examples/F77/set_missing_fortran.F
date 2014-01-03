C Copyright 2005-2014 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C
C
C  Fortran 77 Implementation: set_missing_fortran
C
C  Description: how to set a key as missing.
C
C
C
C
C
      program set
      implicit none
      include 'grib_api_f77.h'
      integer err
      character*6 string_value
      integer infile,outfile
      integer igrib,iret
      character*256 error
      integer len

      infile=5
      outfile=6
      string_value='sfc'
      len=3

      call grib_check(grib_open_file(infile
     X,'../../data/reduced_gaussian_pressure_level.grib2','r'))

      call grib_check(grib_open_file(outfile
     X,'out_surface_level.grib2','w'))

C     a new grib message is loaded from file
C     igrib is the grib id to be used in subsequent calls
      call grib_check(grib_new_from_file(infile,igrib))

      call grib_check(grib_set_string(igrib,
     X'typeOfFirstFixedSurface', string_value))

      call grib_check(grib_set_missing(igrib,
     X'scaleFactorOfFirstFixedSurface'))

      call grib_check(grib_set_missing(igrib,
     X'scaledValueOfFirstFixedSurface'))

C     write modified message to a file
      call grib_check(grib_write(igrib,outfile))

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(infile))

      call grib_check(grib_close_file(outfile))

      end
