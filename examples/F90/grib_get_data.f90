! Copyright 2005-2018 ECMWF.
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
program get_data
use eccodes
implicit none
  integer            :: ifile
  integer            :: iret,i
  real(kind=8),dimension(:),allocatable     :: lats,lons,values
  integer,dimension(:),allocatable          :: bitmap
  integer(4)        :: numberOfPoints
  logical            :: is_missing_value
  integer            :: count1=0, count2=0, bitmapPresent=0, bmp_len=0

  ! Message identifier.
  integer            :: igrib

  ifile=5

  call codes_open_file(ifile, &
       '../../data/reduced_latlon_surface.grib1','R')

  ! Loop on all the messages in a file.
  call codes_grib_new_from_file(ifile,igrib,iret)

  do while (iret/=CODES_END_OF_FILE)
    count1=count1+1
    print *, "===== Message #",count1
    call codes_get(igrib,'numberOfPoints',numberOfPoints)
    call codes_get(igrib,'bitmapPresent',bitmapPresent)

    allocate(lats(numberOfPoints))
    allocate(lons(numberOfPoints))
    allocate(values(numberOfPoints))
    if (bitmapPresent == 1) then
      ! get the bitmap
      call codes_get_size(igrib, 'bitmap', bmp_len)
      allocate(bitmap(bmp_len))
      call codes_get(igrib,'bitmap', bitmap)
    end if

    call codes_grib_get_data(igrib,lats,lons,values)

    do i=1,numberOfPoints
      ! Consult bitmap to see if the i'th value is missing
      is_missing_value=.false.
      if (bitmapPresent == 1 .and. bitmap(i) == 0) then
        is_missing_value=.true.
      end if
      ! Only print non-missing values
      if (.not. is_missing_value) then
        print *, lats(i),lons(i),values(i)
        count2=count2+1
      end if
    enddo
    print *, 'count of non-missing values=',count2
    if (count2 /= 214661) then
      call codes_check(-2, 'incorrect number of missing', '')
    end if

    deallocate(lats)
    deallocate(lons)
    deallocate(values)

    call codes_release(igrib)
    call codes_grib_new_from_file(ifile,igrib, iret)

  end do

  call codes_close_file(ifile)

end program
