! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program codes_set_paths
   use eccodes
   implicit none

   character(len=512) :: samples_path, defs_path

   samples_path = "/tmp/samples"
   defs_path = "/tmp/defs"
   call codes_set_samples_path(samples_path)
   call codes_set_definitions_path(defs_path)

end program
