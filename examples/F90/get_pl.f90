! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to get PL values.
!
!
program get_pl
  use eccodes
  implicit none
  integer                         :: infile
  integer                         :: igrib
  integer                         :: PLPresent, nb_pl
  real, dimension(:), allocatable :: pl

  call codes_open_file(infile, &
       '../../data/reduced_gaussian_surface.grib1','r')

  ! A new grib message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib)

  ! get PLPresent to see if the 'pl' array is there
  call codes_get(igrib,'PLPresent',PLPresent)
  print*, "PLPresent= ", PLPresent
  if (PLPresent == 1) then
    call codes_get_size(igrib,'pl',nb_pl)
    print*, "there are ", nb_pl, " PL values"
    allocate(pl(nb_pl))
    call codes_get(igrib,'pl',pl)
    print*, "pl = ", pl
    deallocate(pl)
  else
    print*, "There is no PL values in your GRIB message!"
  end if
  call codes_release(igrib)

  call codes_close_file(infile)

end program get_pl
