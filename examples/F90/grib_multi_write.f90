! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
!  Description: how to create a multi field message in memory and write
!               it in a file. The multi field messages can be created
!               only in GRIB edition 2.
!
!
!
program grib2_multi_write
  use eccodes
  implicit none
  integer                            :: infile,outfile
  integer                            :: in_gribid
  integer                            :: multi_gribid
  integer                            :: step,startsection

  ! multi field messages can be created only in edition 2 
  call codes_open_file(infile,'../../data/sample.grib2','r')

  call codes_open_file(outfile,'multi_created.grib2','w')

  !     a grib message is loaded from file
  !     in_gribid is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,in_gribid)

  startsection=4
  do step=0,240,12

    call codes_set(in_gribid,"step",step)
    call codes_grib_multi_append(in_gribid,startsection,multi_gribid)

  enddo

!  write messages to a file
  call codes_grib_multi_write(multi_gribid,outfile)

  call codes_release(in_gribid)
  call codes_release(multi_gribid)

  call codes_close_file(infile)
  call codes_close_file(outfile)

end program grib2_multi_write
