! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to use an iterator on lat/lon/values.
!
!
program iterator
use eccodes
implicit none
  integer            :: ifile
  integer            :: iret,iter
  real(kind=8)       :: lat,lon,value,missingValue
  integer            :: n,flags

  ! Message identifier.
  integer            :: igrib

  ifile=5

  call codes_open_file(ifile, &
       '../../data/regular_latlon_surface_constant.grib1','R')

  ! Loop on all the messages in a file.
  call codes_grib_new_from_file(ifile,igrib,iret)

  LOOP: DO WHILE (iret/=CODES_END_OF_FILE)
      ! get as a real8
      call codes_get(igrib, &
           'missingValue',missingValue)
      write(*,*) 'missingValue=',missingValue

      ! A new iterator on lat/lon/values is created from the message igrib
      flags = 0
      call grib_iterator_new(igrib,iter,flags)

      n = 0
      ! Loop on all the lat/lon/values.
      call grib_iterator_next(iter,lat,lon,value, iret)
      do while (iret .ne. 0)
         ! You can now print lat and lon,
         if ( value .eq. missingValue ) then
           ! decide what to print if a missing value is found.
           write(*,*) "- ",n," - lat=",lat," lon=",lon," value=missing"
         else
           ! or print the value if is not missing.
           write(*,*) " ",n," lat=",lat," lon=",lon," value=",value
         endif

         n=n+1

         call grib_iterator_next(iter,lat,lon,value, iret)
      end do

  ! At the end the iterator is deleted to free memory.
  call grib_iterator_delete(iter)
  call codes_release(igrib)

  call codes_grib_new_from_file(ifile,igrib, iret)

  end do LOOP

  call codes_close_file(ifile)

  end program iterator
