! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
! Description: how to set key values.
!
!
program set
  use eccodes
  implicit none
  integer              :: infile,outfile
  integer              :: igrib

  infile=5
  outfile=6

  call codes_open_file(infile, &
       '../../data/reduced_gaussian_pressure_level.grib2','r')

  call codes_open_file(outfile, &
       'out_surface_level.grib2','w')

  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib)

  call codes_set(igrib,'typeOfFirstFixedSurface','sfc')
  call codes_set_missing(igrib,'scaleFactorOfFirstFixedSurface')
  call codes_set_missing(igrib,'scaledValueOfFirstFixedSurface')

  call codes_write(igrib,outfile)

  call codes_release(igrib)

  call codes_close_file(infile)

  call codes_close_file(outfile)

end program set
