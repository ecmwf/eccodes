! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to use codes_grib_find_nearest and codes_get_element 
!
!
!
program find
  use eccodes
  implicit none
  integer                                      :: npoints
  integer                                      :: infile
  integer                                      :: igrib, ios, i
  real(8), dimension(:), allocatable  :: lats, lons
  real(8), dimension(:), allocatable  :: nearest_lats, nearest_lons
  real(8), dimension(:), allocatable  :: distances, values, lsm_values
  integer(kind=kindOfInt), dimension(:), allocatable  :: indexes

! initialization
  open( unit=1, file="../../data/list_points",form="formatted",action="read")
  read(unit=1,fmt=*) npoints
  allocate(lats(npoints))
  allocate(lons(npoints))
  allocate(nearest_lats(npoints))
  allocate(nearest_lons(npoints))
  allocate(distances(npoints))
  allocate(lsm_values(npoints))
  allocate(values(npoints))
  allocate(indexes(npoints))
  do i=1,npoints
     read(unit=1,fmt=*, iostat=ios) lats(i), lons(i)
     if (ios /= 0) then
        npoints = i - 1
        exit
     end if
  end do
  close(unit=1)
  call codes_open_file(infile, &
       '../../data/reduced_gaussian_lsm.grib1','r')
  
  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib)
  

  call codes_grib_find_nearest(igrib, .true., lats, lons, nearest_lats, nearest_lons,lsm_values, distances, indexes)
  call codes_release(igrib)
  
  call codes_close_file(infile)

! will apply it to another GRIB
  call codes_open_file(infile, &
       '../../data/reduced_gaussian_pressure_level.grib1','r')
  call codes_grib_new_from_file(infile,igrib)

  call codes_get_element(igrib,"values", indexes, values)
  call codes_release(igrib)
  call codes_close_file(infile)

  do i=1, npoints
     print*,lats(i), lons(i), nearest_lats(i), nearest_lons(i), distances(i), lsm_values(i), values(i)
  end do

  deallocate(lats)
  deallocate(lons)
  deallocate(nearest_lats)
  deallocate(nearest_lons)
  deallocate(distances)
  deallocate(lsm_values)
  deallocate(values)
  deallocate(indexes)

end program find
