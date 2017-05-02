C Copyright 2005-2017 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C
C
C  Fortran 77 Implementation: keys_iterator
C
C  Description:
C  Example on how to use keys_iterator functions and the
C  grib_keys_iterator structure to get all the available
C  keys in a message.
C
C
C
C
      program keys_iterator
      implicit none
      include 'grib_api_fortran.h'
      character*20 name_space
      integer kiter,ifile,igrib,iret
      character*256 key
      character*256 value
      character*512 all
      integer len,strlen
      integer grib_count
      len=256

      ifile=5

      call grib_check(grib_open_file(ifile,
     X'../data/regular_latlon_surface.grib1','r'))

C     Loop on all the messages in a file.
  10  iret=grib_new_from_file(ifile,igrib)
      if (igrib .eq. -1 )  then
        if (iret .ne.0) then
           call grib_check(iret)
        endif
        stop
      endif

      grib_count=grib_count+1
      write(*,*) '-- GRIB N. ',grib_count,' --'

C     valid name_spaces are ls and mars
      name_space='ls'
C     name_space=' ' to get all the keys */
C     name_space=' '

      strlen = index(name_space,' ') - 1
      call grib_check(
     Xgrib_keys_iterator_new(igrib,kiter,name_space(1:strlen)))
C     call grib_check(grib_keys_iterator_skip_read_only(kiter))
C     call grib_check(grib_keys_iterator_skip_function(kiter))
C     call grib_check(grib_keys_iterator_skip_not_coded(kiter))

  20  if (grib_keys_iterator_next(kiter) .ne. 1) goto 10

      call grib_check(grib_keys_iterator_get_name(kiter,key,strlen))
      call grib_check(grib_get_string(igrib,key(1:strlen),value))
      all=key// ' = ' // value
      write(*,*) all

      goto 20

      call grib_check(grib_keys_iterator_delete(kiter))

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(ifile))

      end

