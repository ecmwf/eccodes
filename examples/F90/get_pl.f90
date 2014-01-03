! Copyright 2005-2014 ECMWF.
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
!
program get_pl
  use grib_api
  implicit none
  integer                         :: infile
  integer                         :: igrib
  integer                         :: PLPresent, nb_pl
  real, dimension(:), allocatable :: pl


  call grib_open_file(infile, &
       '../../data/reduced_gaussian_surface.grib1','r')
  
  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call grib_new_from_file(infile,igrib)
  
  !     set PVPresent as an integer 
  call grib_get(igrib,'PLPresent',PLPresent)
  print*, "PLPresent= ", PLPresent
  if (PLPresent == 1) then
     call grib_get_size(igrib,'pl',nb_pl)
     print*, "there are ", nb_pl, " PL values"
     allocate(pl(nb_pl))
     call grib_get(igrib,'pl',pl)
     print*, "pl = ", pl
     deallocate(pl)
  else
     print*, "There is no PL values in your GRIB message!"
  end if
  call grib_release(igrib)
     
  call grib_close_file(infile)

end program get_pl
