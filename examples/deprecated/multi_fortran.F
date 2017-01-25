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
C  Fortran 77 Implementation: multi_fortran
C
C  Description: How to decode grib messages containing multiple
C               fields. Try to turn on and off multi support to
C               see the difference. Default is OFF.
C         For all the tools defalut is multi support ON.
C
C
C
C
C
      program multi
      implicit none
      include 'grib_api_fortran.h'
      integer iret
      character*256 error
      integer*4 parameterCategory,parameterNumber,discipline
      integer ifile,igrib

      call grib_check( grib_open_file(ifile
     X,'../data/multi.grib2','r'))

C     turn on support for multi fields messages */
      call grib_check(grib_multi_support_on())

C     turn off support for multi fields messages */
C     call grib_check(grib_multi_support_off())

C     Loop on all the messages in a file.
  10  iret=grib_new_from_file(ifile,igrib)
      if (igrib .eq. -1 )  then
        if (iret .ne.0) then
       call grib_check(iret)
        endif
        stop
      endif

C     get as a integer*4
      call grib_check(grib_get_int(igrib,'discipline',discipline))
      write(*,*) 'discipline=',discipline

C     get as a integer*4
      call grib_check(grib_get_int(igrib,'parameterCategory'
     X,parameterCategory))
      write(*,*) 'parameterCategory=',parameterCategory

C     get as a integer*4
      call grib_check(grib_get_int(igrib,'parameterNumber'
     X,parameterNumber))
      write(*,*) 'parameterNumber=',parameterNumber

      if ( discipline .eq. 0 .and. parameterCategory .eq. 2) then
        if (parameterNumber .eq. 2) then
           write(*,*) "-------- u -------"
        endif
        if (parameterNumber .eq. 3) then
           write(*,*) "-------- v -------"
        endif
      endif

      goto 10

      call grib_check(grib_release(igrib))

      call grib_check(grib_close_file(ifile))

      end

