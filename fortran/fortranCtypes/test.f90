integer function kind_of_long_long
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_long_long = -1

  call check_long_long(x2(0), x2(1), ret)
  if (ret == 't') then
    kind_of_long_long = 2
    return
  end if

  call check_long_long(x4(0), x4(1), ret)
  if (ret == 't') then
    kind_of_long_long = 4
    return
  end if

  call check_long_long(x8(0), x8(1), ret)
  if (ret == 't') then
    kind_of_long_long = 8
    return
  end if

end function kind_of_long_long

integer function kind_of_size_t
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_size_t = -1

  call check_size_t(x2(0), x2(1), ret)
  if (ret == 't') then
    kind_of_size_t = 2
    return
  end if

  call check_size_t(x4(0), x4(1), ret)
  if (ret == 't') then
    kind_of_size_t = 4
    return
  end if

  call check_size_t(x8(0), x8(1), ret)
  if (ret == 't') then
    kind_of_size_t = 8
    return
  end if

end function kind_of_size_t

integer function kind_of_long
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_long = -1

  call check_long(x2(0), x2(1), ret)
  if (ret == 't') then
    kind_of_long = 2
    return
  end if

  call check_long(x4(0), x4(1), ret)
  if (ret == 't') then
    kind_of_long = 4
    return
  end if

  call check_long(x8(0), x8(1), ret)
  if (ret == 't') then
    kind_of_long = 8
    return
  end if

end function kind_of_long

integer function kind_of_int
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_int = -1

  call check_int(x2(0), x2(1), ret)
  if (ret == 't') then
    kind_of_int = 2
    return
  end if

  call check_int(x4(0), x4(1), ret)
  if (ret == 't') then
    kind_of_int = 4
    return
  end if

  call check_int(x8(0), x8(1), ret)
  if (ret == 't') then
    kind_of_int = 8
    return
  end if

end function kind_of_int

integer function kind_of_float
  real(4), dimension(2) :: x4 = (/1., 2./)
  real(8), dimension(2) :: x8 = (/1., 2./)
  character(len=1) :: ret

  kind_of_float = -1

  call check_float(x4(0), x4(1), ret)
  if (ret == 't') then
    kind_of_float = 4
    return
  end if

  call check_float(x8(0), x8(1), ret)
  if (ret == 't') then
    kind_of_float = 8
    return
  end if

end function kind_of_float

integer function kind_of_double
  real(4), dimension(2) :: real4 = (/1., 2./)
  real(8), dimension(2) :: real8 = (/1., 2./)
  character(len=1) :: ret

  kind_of_double = -1

  call check_double(real4(0), real4(1), ret)
  if (ret == 't') then
    kind_of_double = 4
    return
  end if

  call check_double(real8(0), real8(1), ret)
  if (ret == 't') then
    kind_of_double = 8
    return
  end if

end function kind_of_double

program test

  print *, 'kind_of_double=', kind_of_double()
  print *, 'kind_of_float=', kind_of_float()
  print *, 'kind_of_int=', kind_of_int()
  print *, 'kind_of_long=', kind_of_long()
  print *, 'kind_of_size_t=', kind_of_size_t()
  print *, 'kind_of_long_long=', kind_of_long_long()

end program test

