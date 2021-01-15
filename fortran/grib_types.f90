! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

integer function kind_of_size_t()
  implicit none
  integer(2), dimension(2) :: x2 = (/1_2, 2_2/)
  integer(4), dimension(2) :: x4 = (/1_4, 2_4/)
  integer(8), dimension(2) :: x8 = (/1_8, 2_8/)
  character(len=1) :: ret

  interface
     subroutine check_size_t_i2(i1,i2,ret)
       implicit none
       integer(2), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_size_t_i2
     subroutine check_size_t_i4(i1,i2,ret)
       implicit none
       integer(4), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_size_t_i4
     subroutine check_size_t_i8(i1,i2,ret)
       implicit none
       integer(8), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_size_t_i8
  end interface
  
  kind_of_size_t=-1

  call check_size_t_i2(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_size_t=2
   return
  endif

  call check_size_t_i4(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_size_t=4
   return
  endif

  call check_size_t_i8(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_size_t=8
   return
  endif

end function kind_of_size_t

integer function kind_of_long()
  implicit none
  integer(2), dimension(2) :: x2 = (/1_2, 2_2/)
  integer(4), dimension(2) :: x4 = (/1_4, 2_4/)
  integer(8), dimension(2) :: x8 = (/1_8, 2_8/)
  character(len=1) :: ret

  interface
     subroutine check_long_i2(i1,i2,ret)
       implicit none
       integer(2), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_long_i2
     subroutine check_long_i4(i1,i2,ret)
       implicit none
       integer(4), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_long_i4
     subroutine check_long_i8(i1,i2,ret)
       implicit none
       integer(8), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_long_i8
  end interface
  
  kind_of_long=-1

  call check_long_i2(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_long=2
   return
  endif

  call check_long_i4(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_long=4
   return
  endif

  call check_long_i8(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_long=8
   return
  endif

end function kind_of_long

integer function kind_of_int()
  implicit none
  integer(2), dimension(2) :: x2 = (/1_2, 2_2/)
  integer(4), dimension(2) :: x4 = (/1_4, 2_4/)
  integer(8), dimension(2) :: x8 = (/1_8, 2_8/)
  character(len=1) :: ret

  interface
     subroutine check_int_i2(i1,i2,ret)
       implicit none
       integer(2), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_int_i2
     subroutine check_int_i4(i1,i2,ret)
       implicit none
       integer(4), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_int_i4
     subroutine check_int_i8(i1,i2,ret)
       implicit none
       integer(8), intent(in) :: i1, i2
       character(len=1), intent(out) :: ret
     end subroutine check_int_i8
  end interface
  
  kind_of_int=-1

  call check_int_i2(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_int=2
   return
  endif

  call check_int_i4(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_int=4
   return
  endif

  call check_int_i8(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_int=8
   return
  endif

end function kind_of_int

integer function kind_of_float()
  implicit none
  real(4), dimension(2) :: x4 = (/1._4, 2._4/)
  real(8), dimension(2) :: x8 = (/1._8, 2._8/)
  character(len=1) :: ret
  
  interface
     subroutine check_float_r4(r1,r2,ret)
       implicit none
       real(4), intent(in) :: r1, r2
       character(len=1), intent(out) :: ret
     end subroutine check_float_r4
     subroutine check_float_r8(r1,r2,ret)
       implicit none
       real(8), intent(in) :: r1, r2
       character(len=1), intent(out) :: ret
     end subroutine check_float_r8
  end interface

  kind_of_float=-1

  call check_float_r4(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_float=4
   return
  endif

  call check_float_r8(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_float=8
   return
  endif

end function kind_of_float

integer function kind_of_double()
  implicit none
  real(4), dimension(2) :: real4 = (/1._4, 2._4/)
  real(8), dimension(2) :: real8 = (/1._8, 2._8/)
  character(len=1) :: ret

  interface
     subroutine check_double_r4(r1,r2,ret)
       implicit none
       real(4), intent(in) :: r1, r2
       character(len=1), intent(out) :: ret
     end subroutine check_double_r4
     subroutine check_double_r8(r1,r2,ret)
       implicit none
       real(8), intent(in) :: r1, r2
       character(len=1), intent(out) :: ret
     end subroutine check_double_r8
  end interface

  kind_of_double=-1

  call check_double_r4(real4(1),real4(2),ret)
  if (ret == 't') then
   kind_of_double=4
   return
  endif

  call check_double_r8(real8(1),real8(2),ret)
  if (ret == 't') then
   kind_of_double=8
   return
  endif

end function kind_of_double

program kind_h
  implicit none
  integer :: size
  integer, dimension(2) :: i
  integer(kind=2), dimension(2) :: i2
  integer(kind=4), dimension(2) :: i4
  real(kind=4), dimension(2) :: r4
  real(kind=8), dimension(2) :: r8
  ! function types
  integer :: kind_of_int
  integer :: kind_of_long
  integer :: kind_of_size_t
  integer :: kind_of_double
  integer :: kind_of_float

  interface
     subroutine f_sizeof(i1,i2,size)
       implicit none
       integer, intent(in) :: i1, i2 ! default integer
       integer, intent(out) :: size
     end subroutine f_sizeof
     subroutine f_sizeof_i2(i1,i2,size)
       implicit none
       integer(kind=2), intent(in) :: i1, i2
       integer, intent(out) :: size
     end subroutine f_sizeof_i2
     subroutine f_sizeof_i4(i1,i2,size)
       implicit none
       integer(kind=4), intent(in) :: i1, i2
       integer, intent(out) :: size
     end subroutine f_sizeof_i4
     subroutine f_sizeof_r4(r1,r2,size)
       implicit none
       real(kind=4), intent(in) :: r1, r2
       integer, intent(out) :: size
     end subroutine f_sizeof_r4
     subroutine f_sizeof_r8(r1,r2,size)
       implicit none
       real(kind=8), intent(in) :: r1, r2
       integer, intent(out) :: size
     end subroutine f_sizeof_r8
  end interface

  ! note: public declarations cannot be added in grib_kinds.h
  ! by the following print statements,
  ! since if you do that, grib_kinds.h cannot be included
  ! inside interface statements
  print *,"integer,parameter :: kindOfInt=",kind_of_int()
  print *,"integer,parameter :: kindOfLong=",kind_of_long()
  print *,"integer,parameter :: kindOfSize_t=",kind_of_size_t()
  print *,"integer,parameter :: kindOfSize=",kind_of_size_t()
  print *,"integer,parameter :: kindOfDouble=",kind_of_double()
  print *,"integer,parameter :: kindOfFloat=",kind_of_float()
  call f_sizeof(i(1),i(2),size)
  print *,"integer,parameter :: sizeOfInteger=",size
  call f_sizeof_i2(i2(1),i2(2),size)
  print *,"integer,parameter :: sizeOfInteger2=",size
  call f_sizeof_i4(i4(1),i4(2),size)
  print *,"integer,parameter :: sizeOfInteger4=",size
  call f_sizeof_r4(r4(1),r4(2),size)
  print *,"integer,parameter :: sizeOfReal4=",size
  call f_sizeof_r8(r8(1),r8(2),size)
  print *,"integer,parameter :: sizeOfReal8=",size

end program kind_h

