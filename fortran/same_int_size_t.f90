! (C) Copyright 2005- ECMWF.
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

  kind_of_size_t = -1

  call check_size_t(x2(1), x2(2), ret)
  if (ret == 't') then
    kind_of_size_t = 2
    return
  end if

  call check_size_t(x4(1), x4(2), ret)
  if (ret == 't') then
    kind_of_size_t = 4
    return
  end if

  call check_size_t(x8(1), x8(2), ret)
  if (ret == 't') then
    kind_of_size_t = 8
    return
  end if

end function kind_of_size_t

integer function kind_of_int()
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_int = -1

  call check_int(x2(1), x2(2), ret)
  if (ret == 't') then
    kind_of_int = 2
    return
  end if

  call check_int(x4(1), x4(2), ret)
  if (ret == 't') then
    kind_of_int = 4
    return
  end if

  call check_int(x8(1), x8(2), ret)
  if (ret == 't') then
    kind_of_int = 8
    return
  end if

end function kind_of_int

program same_int_size_t
  integer ki, kl

  ki = kind_of_int()
  kl = kind_of_size_t()
  if (ki /= kl) then
    write (*, '(i1)') 0
  else
    write (*, '(i1)') 1
  end if
end program same_int_size_t

