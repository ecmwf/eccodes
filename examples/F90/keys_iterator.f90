! Copyright 2005-2014 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description:
!       How to use keys_iterator to get all the available
!       keys in a message.
!
!
!
program keys_iterator
  use grib_api
  implicit none
  character(len=20)  :: name_space
  integer            :: kiter,ifile,igrib,iret
  character(len=256) :: key
  character(len=256) :: value
  character(len=512) :: all1
  integer            :: grib_count
  
  call grib_open_file(ifile, &
       '../../data/regular_latlon_surface.grib1','r')
  
  ! Loop on all the messages in a file.
  
  call grib_new_from_file(ifile,igrib, iret)
  
  do while (iret /= GRIB_END_OF_FILE)

     grib_count=grib_count+1
     write(*,*) '-- GRIB N. ',grib_count,' --'
     
     ! valid name_spaces are ls and mars
     name_space='ls'
     
     call grib_keys_iterator_new(igrib,kiter,name_space)
     
     do
        call grib_keys_iterator_next(kiter, iret) 
        
        if (iret .ne. 1) exit
        
        call grib_keys_iterator_get_name(kiter,key)
        call grib_get(igrib,trim(key),value)
        all1=trim(key)// ' = ' // trim(value)
        write(*,*) trim(all1)
        
     end do
     
     call grib_keys_iterator_delete(kiter)
     call grib_release(igrib)
     call grib_new_from_file(ifile,igrib, iret)
  end do
  
  
  call grib_close_file(ifile)
  
end program keys_iterator

