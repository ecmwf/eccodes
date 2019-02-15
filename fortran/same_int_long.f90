! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

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

program same_int_long
  integer ki,kl

  ki=kind_of_int()
  kl=kind_of_long()
  if (ki /= kl) then
    write (*,'(i1)') 0
  else
    write (*,'(i1)') 1
  endif
end program same_int_long

