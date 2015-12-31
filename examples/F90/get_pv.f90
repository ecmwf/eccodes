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
!  Description: how to get PV values.
!
!
!
program get_pv
  use eccodes
  implicit none
  integer                         :: infile
  integer                         :: igrib
  integer                         :: PVPresent, nb_pv
  real, dimension(:), allocatable :: pv

  call codes_open_file(infile, &
       '../../data/reduced_gaussian_model_level.grib1','r')

  ! A new grib message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib)

  ! Get PVPresent to see if the 'pv' array is there
  call codes_get(igrib,'PVPresent',PVPresent)
  print*, "PVPresent = ", PVPresent
  if (PVPresent == 1) then
    call codes_get_size(igrib,'pv',nb_pv)
    print*, "There are ", nb_pv, " PV values"
    allocate(pv(nb_pv))
    call codes_get(igrib,'pv',pv)
    print*, "pv = ", pv
    deallocate(pv)
  else
    print*, "There is no PV values in your GRIB message!"
  end if
  call codes_release(igrib)

  call codes_close_file(infile)

end program get_pv
