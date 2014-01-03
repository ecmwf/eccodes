! Copyright 2005-2014 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to get lat/lon/values.
!
!
!
program get_data
use grib_api
implicit none
  integer            :: ifile
  integer            :: iret,i
  real(kind=8),dimension(:),allocatable     :: lats,lons,values
  integer(4)        :: numberOfPoints
  real(8)  :: missingValue=9999
  integer           :: count1=0
  character(len=256) :: filename

!     Message identifier.
  integer            :: igrib

  ifile=5

  call grib_open_file(ifile, &
       '../../data/reduced_latlon_surface.grib1','R')

! Loop on all the messages in a file.

  call grib_new_from_file(ifile,igrib,iret)

  do while (iret/=GRIB_END_OF_FILE)
    count1=count1+1
    print *, "===== Message #",count1
    call grib_get(igrib,'numberOfPoints',numberOfPoints)
    call grib_set(igrib,'missingValue',missingValue)

    allocate(lats(numberOfPoints))
    allocate(lons(numberOfPoints))
    allocate(values(numberOfPoints))

    call grib_get_data(igrib,lats,lons,values)

    do i=1,numberOfPoints
      if (values(i) /= missingValue) then
        print *, lats(i),lons(i),values(i)
      end if
    enddo

    deallocate(lats)
    deallocate(lons)
    deallocate(values)

    call grib_release(igrib)
    call grib_new_from_file(ifile,igrib, iret)

  end do 


  call grib_close_file(ifile)

end program 
