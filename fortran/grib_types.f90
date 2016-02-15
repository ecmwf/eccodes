! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

integer function kind_of_size_t()
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_size_t=-1

  call check_size_t(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_size_t=2
   return
  endif

  call check_size_t(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_size_t=4
   return
  endif

  call check_size_t(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_size_t=8
   return
  endif

end function kind_of_size_t

integer function kind_of_long()
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_long=-1

  call check_long(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_long=2
   return
  endif

  call check_long(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_long=4
   return
  endif

  call check_long(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_long=8
   return
  endif

end function kind_of_long

integer function kind_of_int()
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_int=-1

  call check_int(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_int=2
   return
  endif

  call check_int(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_int=4
   return
  endif

  call check_int(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_int=8
   return
  endif

end function kind_of_int

integer function kind_of_float()
  real(4), dimension(2) :: x4 = (/1., 2./)
  real(8), dimension(2) :: x8 = (/1., 2./)
  character(len=1) :: ret

  kind_of_float=-1

  call check_float(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_float=4
   return
  endif

  call check_float(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_float=8
   return
  endif

end function kind_of_float

integer function kind_of_double()
  real(4), dimension(2) :: real4 = (/1., 2./)
  real(8), dimension(2) :: real8 = (/1., 2./)
  character(len=1) :: ret

  kind_of_double=-1

  call check_double(real4(1),real4(2),ret)
  if (ret == 't') then
   kind_of_double=4
   return
  endif

  call check_double(real8(1),real8(2),ret)
  if (ret == 't') then
   kind_of_double=8
   return
  endif

end function kind_of_double

program kind_h
  integer :: size
  integer, dimension(2) :: i
  integer(kind=2), dimension(2) :: i2
  integer(kind=4), dimension(2) :: i4
  real(kind=4), dimension(2) :: r4
  real(kind=8), dimension(2) :: r8

  print *,"integer,public,parameter :: kindOfInt=",kind_of_int()
  print *,"integer,public,parameter :: kindOfLong=",kind_of_long()
  print *,"integer,public,parameter :: kindOfSize_t=",kind_of_size_t()
  print *,"integer,public,parameter :: kindOfSize=",kind_of_size_t()
  print *,"integer,public,parameter :: kindOfDouble=",kind_of_double()
  print *,"integer,public,parameter :: kindOfFloat=",kind_of_float()
  call f_sizeof(i(1),i(2),size)
  print *,"integer,public,parameter :: sizeOfInteger=",size
  call f_sizeof(i2(1),i2(2),size)
  print *,"integer,public,parameter :: sizeOfInteger2=",size
  call f_sizeof(i4(1),i4(2),size)
  print *,"integer,public,parameter :: sizeOfInteger4=",size
  call f_sizeof(r4(1),r4(2),size)
  print *,"integer,public,parameter :: sizeOfReal4=",size
  call f_sizeof(r8(1),r8(2),size)
  print *,"integer,public,parameter :: sizeOfReal8=",size

end program kind_h

