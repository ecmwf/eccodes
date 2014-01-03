C Copyright 2005-2014 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C
C  Fortran 77  Implementation: get_fortran
C
C  Description: how to get values using keys.
C
C
C
C
      program get
      implicit none
      include 'grib_api_f77.h'
      integer ifile
      integer iret
      integer igrib
      integer i
      integer*4 Nj                             
      dimension ifile(10)
      dimension igrib(10)

      iret=grib_open_file(ifile(1),'x.grib1','r')
      call grib_check(iret)

      call grib_check( grib_new_from_file(ifile(1),igrib(1)) )

      iret=grib_open_file(ifile(2),'x.grib1','r')
      call grib_check(iret)

      call grib_check( grib_new_from_file(ifile(2),igrib(2)) )

      print*,"ifile(1)=",ifile(1),"  ifile(2)=",ifile(2)

      iret=grib_open_file(ifile(3),'x.grib1','r')
      call grib_check(iret)

      call grib_check( grib_new_from_file(ifile(3),igrib(3)) )
      call grib_check( grib_new_from_file(ifile(3),igrib(3)) )

      iret=grib_open_file(ifile(4),'x.grib1','r')
      call grib_check(iret)

      call grib_check( grib_new_from_file(ifile(4),igrib(4)) )
      print*,"ifile(3)=",ifile(3),"  ifile(4)=",ifile(4)

      call grib_check(grib_get_int(igrib(1),'Nj',Nj) )
      write(*,*) 'Nj=',Nj
      call grib_check(grib_get_int(igrib(2),'Nj',Nj) )
      write(*,*) 'Nj=',Nj
      call grib_check(grib_get_int(igrib(2),'Nj',Nj) )
      write(*,*) 'Nj=',Nj
      call grib_check(grib_get_int(igrib(3),'Nj',Nj) )
      write(*,*) 'Nj=',Nj



      call grib_check(grib_release(igrib(4)))
      call grib_check(grib_release(igrib(3)))
      call grib_check(grib_release(igrib(2)))
      call grib_check(grib_release(igrib(1)))

      call grib_check(grib_close_file(ifile(1)))
      call grib_check(grib_close_file(ifile(2)))
      call grib_check(grib_close_file(ifile(3)))
      call grib_check(grib_close_file(ifile(4)))


      end
