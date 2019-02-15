! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to copy a namespace from a message to another.
!
!
program copy_namespace
  use eccodes
  implicit none
  integer  :: file1, file2, file3
  integer  :: igrib1,igrib2,igrib3

  call codes_open_file(file1, '../../data/reduced_latlon_surface.grib2', 'r')
  call codes_open_file(file2, '../../data/regular_latlon_surface.grib1', 'r')
  call codes_open_file(file3, 'out.grib_copy_namespace.grib','w')

  call codes_grib_new_from_file(file1, igrib1)
  call codes_grib_new_from_file(file2, igrib2)

  call codes_clone(igrib2, igrib3)

  call codes_copy_namespace(igrib1, 'geography', igrib3)

   call codes_write(igrib3, file3)

  call codes_close_file(file1)
  call codes_close_file(file2)
  call codes_close_file(file3)

end program copy_namespace
